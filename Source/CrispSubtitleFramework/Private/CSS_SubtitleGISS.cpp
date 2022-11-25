// Copyright Crisp Clover.

#include "CSS_SubtitleGISS.h"
#include "CSUserSettings.h"//TODO: move?

void UCSS_SubtitleGISS::Initialize(FSubsystemCollectionBase& collection)
{
	Super::Initialize(collection);

	UGameInstance* gi = GetGameInstance();

	uTimerManager = gi->TimerManager;
	gi->OnLocalPlayerAddedEvent.AddUObject(this, &UCSS_SubtitleGISS::iPlayerAdded);
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

bool UCSS_SubtitleGISS::HasPermanentSubtitle()
{
	for (auto it = iCurrentSubtitles.GetIterator(); it; ++it)
		if (it.IsPermanent())
			return true;

	return false;
}

void UCSS_SubtitleGISS::ModifyQueuedSubtitles(TArray<int32> const& ids, const FName newSource, FText const& newDescription, const bool shouldChangeDescription)
{
	for (const int32 id : ids)
	{
		if (FFullSubtitle* subtitle = iQueuedSubtitles.rAccess(id))
		{
			subtitle->Source = newSource;

			if (shouldChangeDescription)
				subtitle->Description = newDescription;
		}
	}
}

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

void UCSS_SubtitleGISS::RemovePermanents()
{
	TArray<FTimerHandle> handles;
	TArray<int32> ids;

	iCurrentSubtitles.RemovePermanents(handles, ids);

	if (ids.IsEmpty())
		return;

	iManageRemoval(handles, ids);

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

void UCSS_SubtitleGISS::ClearSubtitles()
{
	TArray<FTimerHandle> flushedHandles;
	TArray<int32> flushedIDs;
	iCurrentSubtitles.Flush(flushedHandles, flushedIDs);

	if (flushedIDs.IsEmpty())
		return;

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
		uTimerManager->PauseTimer(handle);

	for (FTimerHandle const& handle : iQueuedSubtitles.Handles())
		uTimerManager->PauseTimer(handle);
}

void UCSS_SubtitleGISS::PauseQueuedSubtitles()
{
	if (!uTimerManager)
		return;

	for (FTimerHandle const& handle : iQueuedSubtitles.Handles())
		uTimerManager->PauseTimer(handle);
}

void UCSS_SubtitleGISS::UnpauseQueuedSubtitles()
{
	if (!uTimerManager)
		return;

	for (FTimerHandle const& handle : iQueuedSubtitles.Handles())
		uTimerManager->UnPauseTimer(handle);
}

void UCSS_SubtitleGISS::UnpauseSubtitles()
{
	iSubtitleBroadcastData.LogUnpaused();

	for (FTimerHandle const& handle : iCurrentSubtitles.Handles())
		uTimerManager->UnPauseTimer(handle);

	for (FTimerHandle const& handle : iQueuedSubtitles.Handles())
		uTimerManager->UnPauseTimer(handle);

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

	if (isPermanent && iCurrentSettings->bShowSubtitles)
		iPauseOnPermanentSubtitle();
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
		ConstructSubtitleEvent.Broadcast(UCSCoreLibrary::FrySubtitle(subtitle, id, iCurrentSettings));
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
	TArray<FCrispSubtitle> subtitles;
	subtitles.Reserve(iCurrentSubtitles.Num());

	for (auto it = iCurrentSubtitles.GetIterator(); it; ++it)
		subtitles.Add(UCSCoreLibrary::FrySubtitle(it.Data(), it.ID(), iCurrentSettings));

	ReconstructSubtitlesEvent.Broadcast(subtitles);
}

#pragma endregion

#pragma region CAPTIONS

bool UCSS_SubtitleGISS::HasPermanentCaption()
{
	for (auto it = iCurrentCaptions.GetIterator(); it; ++it)
		if (it.IsPermanent())
			return true;

	return false;
}

void UCSS_SubtitleGISS::PauseCaptions()
{
	if (!uTimerManager)
		return;

	iCaptionBroadcastData.LogPaused();

	for (FTimerHandle handle : iCurrentCaptions.Handles())
		uTimerManager->PauseTimer(handle);

	for (FTimerHandle const& handle : iQueuedCaptions.Handles())
		uTimerManager->PauseTimer(handle);
}

void UCSS_SubtitleGISS::PauseQueuedCaptions()
{
	if (!uTimerManager)
		return;

	for (FTimerHandle const& handle : iQueuedCaptions.Handles())
		uTimerManager->PauseTimer(handle);
}

void UCSS_SubtitleGISS::UnpauseCaptions()
{
	if (!uTimerManager)
		return;

	iCaptionBroadcastData.LogUnpaused();

	for (FTimerHandle handle : iCurrentCaptions.Handles())
		uTimerManager->UnPauseTimer(handle);

	for (FTimerHandle const& handle : iQueuedCaptions.Handles())
		uTimerManager->UnPauseTimer(handle);

	iDelayedBroadcastCaptions();
}

void UCSS_SubtitleGISS::UnpauseQueuedCaptions()
{
	if (!uTimerManager)
		return;

	for (FTimerHandle const& handle : iQueuedCaptions.Handles())
		uTimerManager->UnPauseTimer(handle);
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

	if (!isPermanent)
	{
		if (dtDisplay < iCurrentSettings->MinimumCaptionTime)
			iSetTimer(&UCSS_SubtitleGISS::iDestroyCaption, handle, id, iCurrentSettings->MinimumCaptionTime);
		else
			iSetTimer(&UCSS_SubtitleGISS::iDestroyCaption, handle, id, dtDisplay);
	}

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

	const bool hadPermanent = HasPermanentSubtitle();

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

	if (hadPermanent && !HasPermanentSubtitle())
		PermanentSubtitlesRemoved.Broadcast();

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

CSIndicatorDelegates* UCSS_SubtitleGISS::rRegisterIndicator(FCSRegisterArgs args, ULocalPlayer const* player)
{
	return iSourcesManager.RegisterIndicator(args, player);
}

void UCSS_SubtitleGISS::UnregisterIndicator(FCSSoundID const& soundID, ULocalPlayer const* player, UObject* widget)
{
	iSourcesManager.UnregisterIndicator(soundID, player, widget);
}

#pragma endregion

#pragma region SETTINGS
void UCSS_SubtitleGISS::RecalculateDesignLayout(const FIntPoint ViewportSize)//TODO
{
	iCurrentSettings->RecalculateDesignLayout(ViewportSize);
}

void UCSS_SubtitleGISS::RecalculateLayout()//TODO
{
	if (UGameViewportClient* viewportClient = GetGameInstance()->GetGameViewportClient())
		iCurrentSettings->RecalculateLayout(viewportClient);
}


void UCSS_SubtitleGISS::SetSettings(UCSUserSettings* settings)
{
	if (!settings || iCurrentSettings == settings)
		return;//We don't want to assign nullptr. Neither do we want to force UI updates when nothing has changed.

	iCurrentSettings = settings;

	/*if (UGameViewportClient* viewportClient = GetGameInstance()->GetGameViewportClient())
		iCurrentSettings->RecalculateLayout(viewportClient);TODO*/

	iAssignedTextColours = settings->AssignedTextColours;
	iShownSpeakers.Empty();

	uReconstructSubtitles();
}

void UCSS_SubtitleGISS::iRecalculateLayout(UGameViewportClient const* viewportClient)//TODO?
{
	//iCurrentSettings->RecalculateLayout(viewportClient);
}

#pragma endregion