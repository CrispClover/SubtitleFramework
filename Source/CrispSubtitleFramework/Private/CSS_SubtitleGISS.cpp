// Copyright Crisp Clover.

#include "CSS_SubtitleGISS.h"
#include "CSCustomDataManager.h"//TODO: move?

void UCSS_SubtitleGISS::Initialize(FSubsystemCollectionBase& collection)
{
	Super::Initialize(collection);

	UGameInstance* gi = GetGameInstance();

	iTimerManager = gi->TimerManager;
	
	gi->OnLocalPlayerAddedEvent.AddUObject(this, &UCSS_SubtitleGISS::uPlayerAdded);
	gi->OnLocalPlayerRemovedEvent.AddUObject(this, &UCSS_SubtitleGISS::iPlayerRemoved);
}

void UCSS_SubtitleGISS::Deinitialize()
{
	UGameInstance* gi = GetGameInstance();
	gi->OnLocalPlayerAddedEvent.RemoveAll(this);
	gi->OnLocalPlayerRemovedEvent.RemoveAll(this);

	Super::Deinitialize();
}

#pragma region SUBTITLES

void UCSS_SubtitleGISS::KickSubtitle()
{
	FTimerHandle handle;
	int32 id;
	iCurrentSubtitles.Kick(handle, id);
	iManageRemoval(handle, id);

	const float tNow = itNow();
	const float dtMissing = iSubtitleBroadcastData.dtFlickerProtectDestruct(tNow, iCurrentSettings->TimeGap);

	if (dtMissing > 0 && !iSubtitleBroadcastData.IsDelayingDestruction())
	{
		iSubtitleBroadcastData.LogDelay(true);
		iSetTimer(&UCSS_SubtitleGISS::iDelayedDestroySubtitles, dtMissing);
	}
	else
	{
		iSubtitleBroadcastData.LogDestruction(tNow);
		DestroySubtitleEvent.Broadcast(id);
	}
}

void UCSS_SubtitleGISS::ClearSubtitles()
{
	TArray<FTimerHandle> flushedHandles;
	TArray<int32> flushedIDs;
	iCurrentSubtitles.Flush(flushedHandles, flushedIDs);
	iManageRemoval(flushedHandles, flushedIDs);

	const float tNow = itNow();
	const float dtMissing = iSubtitleBroadcastData.dtFlickerProtectDestruct(tNow, iCurrentSettings->TimeGap);

	if (dtMissing > 0 && !iSubtitleBroadcastData.IsDelayingDestruction())
	{
		iSubtitleBroadcastData.LogDelay(true);
		iSetTimer(&UCSS_SubtitleGISS::iDelayedDestroySubtitles, dtMissing);
	}
	else
	{
		iSubtitleBroadcastData.LogDestruction(tNow);
		uReconstructSubtitles();
	}
}

void UCSS_SubtitleGISS::PauseSubtitles()
{
	iSubtitleBroadcastData.LogPaused();

	for (FTimerHandle const& handle : iCurrentSubtitles.Handles())
		iTimerManager->PauseTimer(handle);

	for (FTimerHandle const& handle : iQueuedSubtitles.Handles())
		iTimerManager->PauseTimer(handle);
}

void UCSS_SubtitleGISS::UnpauseSubtitles()
{
	iSubtitleBroadcastData.LogUnpaused();

	for (FTimerHandle const& handle : iCurrentSubtitles.Handles())
		iTimerManager->UnPauseTimer(handle);

	for (FTimerHandle const& handle : iQueuedSubtitles.Handles())
		iTimerManager->UnPauseTimer(handle);

	iDelayedBroadcastSubtitles();
}

int32 UCSS_SubtitleGISS::QueueSubtitle(FFullSubtitle const& subtitle)
{
	const int32 id = iIDManager.New();

	if (subtitle.StartDelay == 0)
	{
		//ensure broadcast, even with start time of zero
		iBroadcastSubtitle(subtitle, itNow(), id);
	}
	else
	{
		FTimerHandle& handle = iQueuedSubtitles.Add(subtitle, id);
		iSetTimer(&UCSS_SubtitleGISS::iOnSubtitleTriggered, handle, id, subtitle.StartDelay);
	}

	return id;
}

TArray<int32> UCSS_SubtitleGISS::QueueRawSubtitles(TArray<FRawSubtitle> const& subtitles, FText const& speaker, const FName speakerID, const FName source)
{
	iQueuedSubtitles.Reserve(iQueuedSubtitles.Num() + subtitles.Num());

	TArray<int32> newIDs;
	newIDs.Reserve(subtitles.Num());

	for (const FRawSubtitle subtitle : subtitles)
	{
		const FFullSubtitle fullSub = FFullSubtitle(subtitle, speaker, speakerID, source);
		newIDs.Add(iIDManager.New());

		if (subtitle.StartDelay == 0)
		{
			//ensure broadcast, even with start time of zero
			iBroadcastSubtitle(fullSub, itNow(), newIDs.Last());
		}
		else
		{
			FTimerHandle& handle = iQueuedSubtitles.Add(fullSub, newIDs.Last());
			iSetTimer(&UCSS_SubtitleGISS::iOnSubtitleTriggered, handle, newIDs.Last(), subtitle.StartDelay);
		}
	}

	return newIDs;
}

TArray<int32> UCSS_SubtitleGISS::QueueGroupSubtitles(TArray<FGroupSubtitle> const& subtitles, const FName source)
{
	iQueuedSubtitles.Reserve(iQueuedSubtitles.Num() + subtitles.Num());
	
	TArray<int32> newIDs;
	newIDs.Reserve(subtitles.Num());

	for (const FGroupSubtitle subtitle : subtitles)
	{
		const FFullSubtitle fullSub = FFullSubtitle(subtitle, source);
		newIDs.Add(iIDManager.New());

		if (subtitle.StartDelay == 0)
		{
			//ensure broadcast, even with start time of zero
			iBroadcastSubtitle(fullSub, itNow(), newIDs.Last());
		}
		else
		{
			FTimerHandle& handle = iQueuedSubtitles.Add(fullSub, newIDs.Last());
			iSetTimer(&UCSS_SubtitleGISS::iOnSubtitleTriggered, handle, newIDs.Last(), subtitle.StartDelay);
		}
	}

	return newIDs;
}

TArray<int32> UCSS_SubtitleGISS::QueueFullSubtitles(TArray<FFullSubtitle> const& subtitles)
{
	iQueuedSubtitles.Reserve(iQueuedSubtitles.Num() + subtitles.Num());
	
	TArray<int32> newIDs;
	newIDs.Reserve(subtitles.Num());

	for (const FFullSubtitle subtitle : subtitles)
	{
		newIDs.Add(iIDManager.New());

		if (subtitle.StartDelay == 0)
		{
			//ensure broadcast, even with start time of zero
			iBroadcastSubtitle(subtitle, itNow(), newIDs.Last());
		}
		else
		{
			FTimerHandle& handle = iQueuedSubtitles.Add(subtitle, newIDs.Last());
			iSetTimer(&UCSS_SubtitleGISS::iOnSubtitleTriggered, handle, newIDs.Last(), subtitle.StartDelay);
		}
	}

	return newIDs;
}

void UCSS_SubtitleGISS::RemoveQueuedSubtitle(const int32 id)
{
	FTimerHandle handle = iQueuedSubtitles.Remove(id);
	iManageRemoval(handle, id);
}

void UCSS_SubtitleGISS::RemoveQueuedSubtitles(TArray<int32> const& ids)
{
	for (int32 id : ids)
	{
		FTimerHandle handle = iQueuedSubtitles.Remove(id);
		iManageRemoval(handle, id);
	}
}

void UCSS_SubtitleGISS::RemoveQueuedSubtitlesBySource(const FName source)
{
	for (auto it = iQueuedSubtitles.GetIterator(); it; ++it)
	{
		if (it.Data().Source == source)
		{
			iManageRemoval(it.Handle(), it.ID());
			it.RemoveCurrent();
		}
	}
}

void UCSS_SubtitleGISS::iOnSubtitleTriggered(const int32 id)
{
	iBroadcastSubtitle(iQueuedSubtitles.Consume(id), itNow(), id);
}

void UCSS_SubtitleGISS::iBroadcastSubtitle(FFullSubtitle const& subtitle, const float tNow, const int32 id)
{
	const float dtMissing = iSubtitleBroadcastData.dtFlickerProtectConstruct(tNow, iCurrentSettings->TimeGap);

	if (dtMissing > 0)
		iDelaySubtitle(subtitle, id, dtMissing);
	else
		uBroadcastSubtitle(subtitle, tNow, id);
}

//Unsafe to call without checking flicker protection
void UCSS_SubtitleGISS::uBroadcastSubtitle(FFullSubtitle const& subtitle, const float tNow, const int32 id)
{
	if (!iSourcesManager.GetSources().Contains(subtitle.Source))
		return;//If the source isn't registered (or is excluded due to SourceOverride) we don't want to broadcast.

	const bool isPermanent = iCurrentSettings->ReadingSpeed < 0 || subtitle.ReadDuration < 0;

	const float dtDisplay =
		  FMath::Abs(subtitle.ReadDuration * iCurrentSettings->ReadingSpeed)
		+ iSubtitleBroadcastData.dtBusy(tNow);

	TArray<FTimerHandle> removedHandles = TArray<FTimerHandle>();
	TArray<int32> removedIDs = TArray<int32>();

	FTimerHandle& handle = iCurrentSubtitles.Add(subtitle, id, isPermanent, removedHandles, removedIDs);
	iManageRemoval(removedHandles, removedIDs);

	if (isPermanent)
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, TEXT("Must be skipped."));//TODO: skipping logic, no timer
	else if (dtDisplay < iCurrentSettings->MinimumSubtitleTime)
		iSetTimer(&UCSS_SubtitleGISS::iDestroySubtitle, handle, id, iCurrentSettings->MinimumSubtitleTime);
	else
		iSetTimer(&UCSS_SubtitleGISS::iDestroySubtitle, handle, id, dtDisplay);

	if (!iCurrentSettings->bShowSubtitles)
		return;//We will track subtitles without displaying them.
	
	iSubtitleBroadcastData.LogBroadcast(tNow, dtDisplay);

	if (removedHandles.Num())
		uReconstructSubtitles();
	else
		ConstructSubtitleEvent.Broadcast(UCSLibrary::FrySubtitle(subtitle, id, iCurrentSettings));
}

void UCSS_SubtitleGISS::iDelaySubtitle(FFullSubtitle const& subtitle, const int32 id, const float dtMissing)
{
	if (!iDelayedSubtitles.Num())
		iSetTimer(&UCSS_SubtitleGISS::iDelayedBroadcastSubtitles, dtMissing);

	iDelayedSubtitles.Add(id, subtitle);
}

void UCSS_SubtitleGISS::iDelayedBroadcastSubtitles()
{
	if (iSubtitleBroadcastData.IsPaused())
		return;//We will attempt to broadcast again when UnpauseSubtitles is called.

	const float tNow = itNow();
	const float dtMissing = iSubtitleBroadcastData.dtFlickerProtectConstruct(tNow, iCurrentSettings->TimeGap);

	if (dtMissing > 0)
		iSetTimer(&UCSS_SubtitleGISS::iDelayedBroadcastSubtitles, dtMissing);
	else
		for (TPair<int32, FFullSubtitle> const& subtitle : iDelayedSubtitles)
			uBroadcastSubtitle(subtitle.Value, tNow, subtitle.Key);
}

void UCSS_SubtitleGISS::iDestroySubtitle(const int32 id)
{
	const float tNow = itNow();
	const float dtMissing = iSubtitleBroadcastData.dtFlickerProtectDestruct(tNow, iCurrentSettings->TimeGap);

	iCurrentSubtitles.Remove(id);

	if (CustomData)
		CustomData->RemoveData(id);

	iIDManager.Delete(id);

	if (dtMissing > 0 && !iSubtitleBroadcastData.IsDelayingDestruction())
	{
		iSubtitleBroadcastData.LogDelay(true);
		iSetTimer(&UCSS_SubtitleGISS::iDelayedDestroySubtitles, dtMissing);
	}
	else
	{
		iSubtitleBroadcastData.LogDestruction(tNow);
		DestroySubtitleEvent.Broadcast(id);
	}
}

void UCSS_SubtitleGISS::iDelayedDestroySubtitles()
{
	const float tNow = itNow();
	const float dtMissing = iSubtitleBroadcastData.dtFlickerProtectDestruct(tNow, iCurrentSettings->TimeGap);

	if (dtMissing > 0)
	{
		iSetTimer(&UCSS_SubtitleGISS::iDelayedDestroySubtitles, dtMissing);
	}
	else
	{
		iSubtitleBroadcastData.LogDelay(false);
		iSubtitleBroadcastData.LogDestruction(tNow);
		uReconstructSubtitles();
	}
}

//Unsafe to call without checking flicker protection
void UCSS_SubtitleGISS::uReconstructSubtitles() const
{
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, TEXT("Reconstructing"));

	TArray<FCrispSubtitle> subtitles;
	subtitles.Reserve(iCurrentSubtitles.Num());

	for (auto it = iCurrentSubtitles.GetIterator(); it; ++it)
		subtitles.Add(UCSLibrary::FrySubtitle(it.Data(), it.ID(), iCurrentSettings));

	ReconstructSubtitlesEvent.Broadcast(subtitles);
}
#pragma endregion

#pragma region CAPTIONS
void UCSS_SubtitleGISS::PauseCaptions()
{
	iCaptionBroadcastData.LogPaused();

	for (FTimerHandle handle : iCurrentCaptions.Handles())
		iTimerManager->PauseTimer(handle);

	for (FTimerHandle const& handle : iQueuedCaptions.Handles())
		iTimerManager->PauseTimer(handle);
}

void UCSS_SubtitleGISS::UnpauseCaptions()
{
	iCaptionBroadcastData.LogUnpaused();

	 

	for (FTimerHandle handle : iCurrentCaptions.Handles())
		iTimerManager->UnPauseTimer(handle);

	for (FTimerHandle const& handle : iQueuedCaptions.Handles())
		iTimerManager->UnPauseTimer(handle);

	iDelayedBroadcastCaptions();
}

int32 UCSS_SubtitleGISS::QueueCaption(FFullCaption const& caption)
{
	const int32 id = iIDManager.New();

	if (caption.StartDelay == 0)
	{
		//ensure broadcast, even with start time of zero
		iBroadcastCaption(caption, itNow(), id);
	}
	else
	{
		FTimerHandle& handle = iQueuedCaptions.Add(caption, id);
		iSetTimer(&UCSS_SubtitleGISS::iOnCaptionTriggered, handle, id, caption.StartDelay);
	}

	return id;
}

TArray<int32> UCSS_SubtitleGISS::QueueCaptions(TArray<FSoundCaption> const& captions, const FName source, const FName sound, const float dtDisplay)
{
	iQueuedCaptions.Reserve(iQueuedCaptions.Num() + captions.Num());

	TArray<int32> newIDs;
	newIDs.Reserve(captions.Num());

	for (FSoundCaption const& caption : captions)
	{
		const int32 id = iIDManager.New();

		FFullCaption fullCap = FFullCaption(caption, source, sound, dtDisplay);
		if (fullCap.StartDelay == 0)
		{
			//ensure broadcast, even with start time of zero
			iBroadcastCaption(fullCap, itNow(), id);
		}
		else
		{
			FTimerHandle& handle = iQueuedCaptions.Add(fullCap, id);
			iSetTimer(&UCSS_SubtitleGISS::iOnCaptionTriggered, handle, id, caption.StartDelay);
			newIDs.Add(id);
		}
	}

	return newIDs;
}

TArray<int32> UCSS_SubtitleGISS::QueueFullCaptions(TArray<FFullCaption> const& captions)
{
	iQueuedCaptions.Reserve(iQueuedCaptions.Num() + captions.Num());
	
	TArray<int32> newIDs;
	newIDs.Reserve(captions.Num());

	for (FFullCaption const& caption : captions)
	{
		const int32 id = iIDManager.New();

		if (caption.StartDelay == 0)
		{
			//ensure broadcast, even with start time of zero
			iBroadcastCaption(caption, itNow(), id);
		}
		else
		{
			FTimerHandle& handle = iQueuedCaptions.Add(caption, id);
			iSetTimer(&UCSS_SubtitleGISS::iOnCaptionTriggered, handle, id, caption.StartDelay);
			newIDs.Add(id);
		}
	}
	
	return newIDs;
}

void UCSS_SubtitleGISS::RemoveQueuedCaption(int32 id)
{
	FTimerHandle handle = iQueuedCaptions.Remove(id);
	iManageRemoval(handle, id);
}

void UCSS_SubtitleGISS::RemoveQueuedCaptions(TArray<int32> const& ids)
{
	for (int32 id : ids)
	{
		FTimerHandle handle = iQueuedCaptions.Remove(id);
		iManageRemoval(handle, id);
	}
}

void UCSS_SubtitleGISS::RemoveQueuedCaptionsBySource(const FName source)
{
	for (auto it = iQueuedCaptions.GetIterator(); it; ++it)
	{
		if (it.Data().SoundID.Source == source)
		{
			iManageRemoval(it.Handle(), it.ID());
			it.RemoveCurrent();
		}
	}
}

void UCSS_SubtitleGISS::iOnCaptionTriggered(const int32 id)
{
	iBroadcastCaption(iQueuedCaptions.Consume(id), itNow(), id);
}

void UCSS_SubtitleGISS::iBroadcastCaption(FFullCaption const& caption, const float tNow, const int32 id)
{
	if (bCaptionsAsSubtitles)
	{
		iBroadcastSubtitle(FFullSubtitle(caption), tNow, id);//TODO?
	}
	else
	{
		const float dtMissing = iCaptionBroadcastData.dtFlickerProtectConstruct(tNow, iCurrentSettings->TimeGap);

		if (dtMissing > 0)
			iDelayCaption(caption, id, dtMissing);
		else
			uBroadcastCaption(caption, tNow, id);
	}
}

//Unsafe to call without checking flicker protection
void UCSS_SubtitleGISS::uBroadcastCaption(FFullCaption const& caption, const float tNow, const int32 id)
{
	if (!iSourcesManager.GetSources().Contains(caption.SoundID.Source))
		return;//If the source isn't registered (or is excluded due to the SourceOverride) we don't want to broadcast.

	const bool isPermanent = caption.DisplayDuration < 0;

	const float dtDisplay = FMath::Abs(caption.DisplayDuration);

	FTimerHandle& handle = iCurrentCaptions.Add(caption, id, isPermanent);

	if (isPermanent)
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, TEXT("Will stay."));//TODO: skipping logic, no timer
	else if (dtDisplay < iCurrentSettings->MinimumCaptionTime)
		iSetTimer(&UCSS_SubtitleGISS::iDestroyCaption, handle, id, iCurrentSettings->MinimumCaptionTime);
	else
		iSetTimer(&UCSS_SubtitleGISS::iDestroyCaption, handle, id, dtDisplay);

	if (!iCurrentSettings->bShowCaptions)
		return;//We will track captions without displaying them.

	iCaptionBroadcastData.LogConstruction(tNow);
	ConstructCaptionEvent.Broadcast(FCrispCaption(caption, id));//TODO
}

void UCSS_SubtitleGISS::iDelayCaption(FFullCaption const& caption, const int32 id, const float dtMissing)
{
	if (!iDelayedCaptions.Num())
		iSetTimer(&UCSS_SubtitleGISS::iDelayedBroadcastCaptions, dtMissing);

	iDelayedCaptions.Add(id, caption);
}

void UCSS_SubtitleGISS::iDelayedBroadcastCaptions()
{
	if (iCaptionBroadcastData.IsPaused())
		return;//We will attempt to broadcast again when UnpauseCaptions is called.

	const float tNow = itNow();
	const float dtMissing = iCaptionBroadcastData.dtFlickerProtectConstruct(tNow, iCurrentSettings->TimeGap);

	if (dtMissing > 0)
		iSetTimer(&UCSS_SubtitleGISS::iDelayedBroadcastCaptions, dtMissing);
	else
		for (TPair<int32, FFullCaption> const& caption : iDelayedCaptions)
			uBroadcastCaption(caption.Value, tNow, caption.Key);
}

void UCSS_SubtitleGISS::iDestroyCaption(const int32 id)
{
	iCurrentCaptions.Remove(id);
	DestructCaptionEvent.Broadcast(id);
}

void UCSS_SubtitleGISS::iDelayedDestroyCaptions()
{
	const float tNow = itNow();
	const float dtMissing = iCaptionBroadcastData.dtFlickerProtectDestruct(tNow, iCurrentSettings->TimeGap);

	if (dtMissing > 0)
	{
		iSetTimer(&UCSS_SubtitleGISS::iDelayedDestroyCaptions, dtMissing);
	}
	else
	{
		iCaptionBroadcastData.LogDelay(false);
		iCaptionBroadcastData.LogDestruction(tNow);
		uReconstructCaptions();
	}
}

//Unsafe to call without checking flicker protection
void UCSS_SubtitleGISS::uReconstructCaptions() const
{
	TArray<FCrispCaption> captions;
	captions.Reserve(iCurrentCaptions.Num());

	for (auto it = iCurrentCaptions.GetIterator(); it; ++it)
		captions.Add(FCrispCaption(it.Data(), it.ID()));

	ReconstructCaptionsEvent.Broadcast(captions);
}
#pragma endregion

#pragma region DATA ACCESS
float UCSS_SubtitleGISS::GetBusyDuration() const
{
	return iSubtitleBroadcastData.dtBusy(itNow());
}

bool UCSS_SubtitleGISS::HasPermanentSubtitle() const
{
	return false;//TODO: loop over current subtitles
}
#pragma endregion

#pragma region SOURCE MANAGEMENT
bool UCSS_SubtitleGISS::RegisterSource(const FName name)
{
	return iSourcesManager.AddSource(name);
}

bool UCSS_SubtitleGISS::RegisterAutoNamedSource(AActor const* source, FName& name)
{
	name = source->GetFName();
	return iSourcesManager.AddSource(name);
}

bool UCSS_SubtitleGISS::RegisterAndTrackSound3D(FCSSoundID const& soundID, FVector const& location, ULocalPlayer const* player)
{
	if (!RegisterSource(soundID.Source))
		return false;

	iSourcesManager.TrackSound(soundID, location, player);
	return true;
}

bool UCSS_SubtitleGISS::RegisterAndTrackSound2D(FCSSoundID const& soundID, FVector2D const& position, ULocalPlayer const* player)
{
	if (!RegisterSource(soundID.Source))
		return false;

	//return sourcesManager.StartTrackingSource(name, position, player);//TODO
	return true;
}

bool UCSS_SubtitleGISS::TrackSound3D(FCSSoundID const& soundID, FVector const& location, ULocalPlayer const* player)
{
	return iSourcesManager.TrackSound(soundID, location, player);
}

bool UCSS_SubtitleGISS::TrackSound2D(FCSSoundID const& soundID, FVector2D const& position, ULocalPlayer const* player)
{
	return false;// sourcesManager.TrackSound(name, position, player);//TODO
}

bool UCSS_SubtitleGISS::GetSoundLocation(FCSSoundID const& soundID,  FVector& location, ULocalPlayer const* player) const
{
	return iSourcesManager.GetSoundData(soundID, location, player);
}

void UCSS_SubtitleGISS::StopTrackingSound(FCSSoundID const& soundID, ULocalPlayer const* player)
{
	//iSourcesManager.StopTrackingSource(name, player);//TODO: update for sound
}

bool UCSS_SubtitleGISS::UnregisterSource(const FName source)
{
	if (!iSourcesManager.RemoveSource(source))
		return false;

	RemoveQueuedSubtitlesBySource(source);
	RemoveQueuedCaptionsBySource(source);

	for (auto it = iDelayedSubtitles.CreateIterator(); it; ++it)
		if (it.Value().Source == source)
			it.RemoveCurrent();

	TArray<FTimerHandle> removedHandles = TArray<FTimerHandle>();
	TArray<int32> removedIDs = TArray<int32>();

	iCurrentSubtitles.RemoveBySource(source, removedHandles, removedIDs);

	if (removedHandles.IsEmpty())
		return true;

	iManageRemoval(removedHandles, removedIDs);

	iSubtitleBroadcastData.LogDestruction(itNow());
	uReconstructSubtitles();

	return true;
}

void UCSS_SubtitleGISS::EmptySources()
{
	iSourcesManager.EmptySources();
}

bool UCSS_SubtitleGISS::IsSoundTracked(FCSSoundID const& soundID, ULocalPlayer const* player) const
{
	return iSourcesManager.IsSoundTracked(soundID, player);
}

TSet<FName> UCSS_SubtitleGISS::GetSourceNames() const
{
	return iSourcesManager.GetSources();
}

bool UCSS_SubtitleGISS::HasSourcesOverride() const
{
	return iSourcesManager.HasOverride();
}

void UCSS_SubtitleGISS::SetSourcesOverride(TSet<FName> const& sources)
{
	iSourcesManager.SetSourcesOverride(sources);
}

void UCSS_SubtitleGISS::ClearSourcesOverride()
{
	iSourcesManager.ClearSourcesOverride();
}
#pragma endregion

#pragma region INDICATORS
void UCSS_SubtitleGISS::UpdateIndicatorData(ULocalPlayer const* player)
{
	iSourcesManager.CallCalculate(player);
}

CSIndicatorDelegates* UCSS_SubtitleGISS::RegisterIndicator(FCSRegisterArgs args, ULocalPlayer const* player)
{
	return iSourcesManager.RegisterIndicator(args, player);
}

void UCSS_SubtitleGISS::UnregisterIndicator(FCSSoundID const& soundID, ULocalPlayer const* player, UObject* widget)
{
	iSourcesManager.UnregisterIndicator(soundID, player, widget);
}

#pragma endregion

#pragma region SETTINGS

void UCSS_SubtitleGISS::LoadSettings(FString const& path)
{
	if (!iSettingsLibrary)
		iSettingsLibrary = UObjectLibrary::CreateLibrary(UCSUserSettings::StaticClass(), false, GIsEditor);

	iSettingsLibrary->LoadAssetDataFromPath(path);
	iSettingsLibrary->LoadAssetsFromAssetData();
}

bool UCSS_SubtitleGISS::LoadSettingsAsync(FStreamableDelegate delegateToCall)
{
	if (!iSettingsLibrary)
		iSettingsLibrary = UObjectLibrary::CreateLibrary(UCSUserSettings::StaticClass(), false, GIsEditor);

	const int32 numSettings = iSettingsLibrary->LoadAssetDataFromPath(GetDefault<UCSProjectSettings>()->SettingsPath.Path);

	TArray<FSoftObjectPath> settingsPaths;
	settingsPaths.Reserve(numSettings);

	TArray<FAssetData> assetDataList;
	iSettingsLibrary->GetAssetDataList(assetDataList);

	for (FAssetData assetData : assetDataList)
		if (!assetData.IsAssetLoaded())
			settingsPaths.Add(assetData.ToSoftObjectPath());

	if (!settingsPaths.Num())
		return false;

	FStreamableManager& streamable = UAssetManager::GetStreamableManager();
	streamable.RequestAsyncLoad(settingsPaths, delegateToCall);

	return true;
}

TArray<UCSUserSettings*> UCSS_SubtitleGISS::GetSettingsList()
{
	if (!iSettingsLibrary)
		LoadSettings(GetDefault<UCSProjectSettings>()->SettingsPath.Path);

	return uGetSettingsList();
}

void UCSS_SubtitleGISS::SetSettings(UCSUserSettings* settings)
{
	if (!settings || iCurrentSettings->ID == settings->ID)
		return;//We don't want to assign nullptr. Neither do we want to force UI updates when nothing has changed.

	uSetSettings(settings);
	//TODO: need to calculate layout.
}

void UCSS_SubtitleGISS::ChangeSettingsAsync(FName settingsID)
{
	if (iCurrentSettings->ID == settingsID)
		return;//We don't want to force UI updates when nothing has changed.

	LoadSettingsAsync(FStreamableDelegate::CreateUObject(this, &UCSS_SubtitleGISS::uSetSettingsByID, settingsID));
}

#pragma endregion