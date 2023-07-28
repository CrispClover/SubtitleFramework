// Copyright Crisp Clover.

#include "CSS_SubtitleGISS.h"
#include "CSUserSettings.h"

void UCSS_SubtitleGISS::Initialize(FSubsystemCollectionBase& collection)
{
	Super::Initialize(collection);

	UGameInstance* gi = GetGameInstance();

	uTimerManager = gi->TimerManager;
	gi->OnLocalPlayerAddedEvent.AddUObject(this, &UCSS_SubtitleGISS::iPlayerAdded);
	gi->OnLocalPlayerRemovedEvent.AddUObject(this, &UCSS_SubtitleGISS::iPlayerRemoved);

	TSubclassOf<UCSCustomDataManager> cdmClass = GetDefault<UCSProjectSettings>()->CustomDataClass;

	if (cdmClass)
	{
		CustomDataManager = NewObject<UCSCustomDataManager>(this, cdmClass.Get());
	}
}

void UCSS_SubtitleGISS::Deinitialize()
{
	UGameInstance* gi = GetGameInstance();

	gi->OnLocalPlayerAddedEvent.RemoveAll(this);
	gi->OnLocalPlayerRemovedEvent.RemoveAll(this);

	Super::Deinitialize();
}

#pragma region SUBTITLES

TArray<FFullSubtitle> UCSS_SubtitleGISS::GetUpcomingSubtitles(float durationToSearch) const
{
	if (!uTimerManager)
		return TArray<FFullSubtitle>();

	TArray<FFullSubtitle> results = TArray<FFullSubtitle>();

	TArray<FFullSubtitle> const& subtitles = iQueuedSubtitles.Get();
	TArray<FTimerHandle> const& handles = iQueuedSubtitles.Handles();

	int32 c = iQueuedSubtitles.Num();
	for (int32 x = 0; x < c; x++)
	{
		if (uTimerManager->GetTimerRemaining(handles[x]) < durationToSearch)
		{
			results.Add(subtitles[x]);
		}
	}

	return results;
}

bool UCSS_SubtitleGISS::HasPermanentSubtitle()
{
	for (auto it = iCurrentSubtitles.GetIterator(); it; ++it)
	{
		if (it.IsPermanent())
		{
			return true;
		}
	}

	return false;
}

void UCSS_SubtitleGISS::ModifyQueuedSubtitles(TArray<int32> const& ids, const FName newSource, FText const& newDescription, const bool shouldChangeDescription)
{
	for (const int32 id : ids)
	{
		if (FFullSubtitle* subtitle = iQueuedSubtitles.uAccess(id))
		{
			subtitle->Source = newSource;

			if (shouldChangeDescription)
			{
				subtitle->Description = newDescription;
			}
		}
	}
}

void UCSS_SubtitleGISS::KickSubtitle()
{
	FTimerHandle handle;
	int32 id;
	iCurrentSubtitles.Kick(handle, id);

	iManageRemoval(handle, id);
	
	DestructSubtitleEvent.Broadcast(id);
}

void UCSS_SubtitleGISS::RemovePermanents()
{
	TArray<FTimerHandle> removedHandles;
	TArray<int32> removedIDs;
	iCurrentSubtitles.RemovePermanents(removedHandles, removedIDs);

	if (removedIDs.IsEmpty())
		return;

	iManageRemoval(removedHandles, removedIDs);

	for (const int32 id : removedIDs)
		DestructSubtitleEvent.Broadcast(id);
}

void UCSS_SubtitleGISS::ClearSubtitles()
{
	TArray<FTimerHandle> flushedHandles;
	TArray<int32> flushedIDs;
	iCurrentSubtitles.Flush(flushedHandles, flushedIDs);

	if (flushedIDs.IsEmpty())
		return;

	iManageRemoval(flushedHandles, flushedIDs);

	for (const int32 id : flushedIDs)
		DestructSubtitleEvent.Broadcast(id);
}

void UCSS_SubtitleGISS::PauseSubtitles()
{
	if (!uTimerManager)
		return;

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

void UCSS_SubtitleGISS::UnpauseSubtitles()
{
	if (!uTimerManager)
		return;

	iSubtitleBroadcastData.LogUnpaused();

	for (FTimerHandle const& handle : iCurrentSubtitles.Handles())
		uTimerManager->UnPauseTimer(handle);

	for (FTimerHandle const& handle : iQueuedSubtitles.Handles())
		uTimerManager->UnPauseTimer(handle);
}

void UCSS_SubtitleGISS::UnpauseQueuedSubtitles()
{
	if (!uTimerManager)
		return;

	for (FTimerHandle const& handle : iQueuedSubtitles.Handles())
		uTimerManager->UnPauseTimer(handle);
}

int32 UCSS_SubtitleGISS::QueueSubtitle(FFullSubtitle const& subtitle)
{
	const int32 id = iIDManager.New();

	if (subtitle.StartDelay == 0)
	{
		//ensure broadcast, even with start time of zero
		iBroadcastSubtitle(subtitle, tNow(), id);
	}
	else
	{
		FTimerHandle& handle = iQueuedSubtitles.Add(subtitle, id);
		iSetTimer(&UCSS_SubtitleGISS::iOnSubtitleTriggered, handle, id, subtitle.StartDelay);
	}

	return id;
}

TArray<int32> UCSS_SubtitleGISS::QueueRawSubtitles(TArray<FRawSubtitle> const& subtitles, FText const& speakerText, const FName speaker, const FName source)
{
	iQueuedSubtitles.Reserve(iQueuedSubtitles.Num() + subtitles.Num());

	TArray<int32> newIDs;
	newIDs.Reserve(subtitles.Num());

	for (FRawSubtitle const& subtitle : subtitles)
	{
		const FFullSubtitle fullSub = FFullSubtitle(subtitle, speakerText, speaker, source);
		const int32 id = iIDManager.New();
		newIDs.Add(id);

		if (subtitle.StartDelay == 0)
		{
			//ensure broadcast, even with start time of zero
			iBroadcastSubtitle(fullSub, tNow(), id);
		}
		else
		{
			FTimerHandle& handle = iQueuedSubtitles.Add(fullSub, id);
			iSetTimer(&UCSS_SubtitleGISS::iOnSubtitleTriggered, handle, id, subtitle.StartDelay);
		}
	}

	return newIDs;
}

TArray<int32> UCSS_SubtitleGISS::QueueGroupSubtitles(TArray<FGroupSubtitle> const& subtitles, const FName source)
{
	iQueuedSubtitles.Reserve(iQueuedSubtitles.Num() + subtitles.Num());
	
	TArray<int32> newIDs;
	newIDs.Reserve(subtitles.Num());

	for (FGroupSubtitle const& subtitle : subtitles)
	{
		const FFullSubtitle fullSub = FFullSubtitle(subtitle, source);
		const int32 id = iIDManager.New();
		newIDs.Add(id);

		if (subtitle.StartDelay == 0)
		{
			//ensure broadcast, even with start time of zero
			iBroadcastSubtitle(fullSub, tNow(), id);
		}
		else
		{
			FTimerHandle& handle = iQueuedSubtitles.Add(fullSub, id);
			iSetTimer(&UCSS_SubtitleGISS::iOnSubtitleTriggered, handle, id, subtitle.StartDelay);
		}
	}

	return newIDs;
}

TArray<int32> UCSS_SubtitleGISS::QueueFullSubtitles(TArray<FFullSubtitle> const& subtitles)
{
	iQueuedSubtitles.Reserve(iQueuedSubtitles.Num() + subtitles.Num());
	
	TArray<int32> newIDs;
	newIDs.Reserve(subtitles.Num());

	for (FFullSubtitle const& subtitle : subtitles)
	{
		const int32 id = iIDManager.New();
		newIDs.Add(id);

		if (subtitle.StartDelay == 0)
		{
			//ensure broadcast, even with start time of zero
			iBroadcastSubtitle(subtitle, tNow(), id);
		}
		else
		{
			FTimerHandle& handle = iQueuedSubtitles.Add(subtitle, id);
			iSetTimer(&UCSS_SubtitleGISS::iOnSubtitleTriggered, handle, id, subtitle.StartDelay);
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
	for (const int32 id : ids)
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
	iBroadcastSubtitle(iQueuedSubtitles.Consume(id), tNow(), id);
}

//Unsafe to call without checking flicker protection
void UCSS_SubtitleGISS::iBroadcastSubtitle(FFullSubtitle const& subtitle, const float tNow, const int32 id)
{
	if (!iSourcesManager.GetSources().Contains(subtitle.Source))
		return;//If the source isn't registered (or is excluded due to SourceOverride) we don't want to broadcast.

	const bool isPermanent = iCurrentSettings->ReadingSpeed < 0 || subtitle.ReadDuration < 0;

	const float dtDisplay
		= iCurrentSettings->bAccumulateReadTime
		? FMath::Abs(subtitle.ReadDuration * iCurrentSettings->ReadingSpeed) + iSubtitleBroadcastData.dtBusy(tNow)
		: FMath::Abs(subtitle.ReadDuration * iCurrentSettings->ReadingSpeed);

	TArray<FTimerHandle> removedHandles = TArray<FTimerHandle>();
	TArray<int32> removedIDs = TArray<int32>();

	FTimerHandle& handle = iCurrentSubtitles.Add(subtitle, id, isPermanent, removedHandles, removedIDs);
	iManageRemoval(removedHandles, removedIDs);

	if (isPermanent && iCurrentSettings->bShowSubtitles)
	{
		iPauseOnPermanentSubtitle();
	}
	else if (dtDisplay < iCurrentSettings->MinimumSubtitleTime)
	{
		iSetTimer(&UCSS_SubtitleGISS::iDestroySubtitle, handle, id, iCurrentSettings->MinimumSubtitleTime);
	}
	else
	{
		iSetTimer(&UCSS_SubtitleGISS::iDestroySubtitle, handle, id, dtDisplay);
	}

	if (!iCurrentSettings->bShowSubtitles)
		return;//We will track subtitles without displaying them.
	
	iSubtitleBroadcastData.LogBroadcast(tNow, dtDisplay);

	for (const int32 remID : removedIDs)
		DestructSubtitleEvent.Broadcast(remID);
	
	ConstructSubtitleEvent.Broadcast(UCSCoreLibrary::FrySubtitle(subtitle, id, iCurrentSettings));
}

void UCSS_SubtitleGISS::iDestroySubtitle(const int32 id)
{
	iCurrentSubtitles.Remove(id);
	iManageRemoval(id);
	
	DestructSubtitleEvent.Broadcast(id);
}

void UCSS_SubtitleGISS::iReconstructSubtitles() const
{
	TArray<FCrispSubtitle> subtitles;
	subtitles.Reserve(iCurrentSubtitles.Num());

	for (auto it = iCurrentSubtitles.GetIterator(); it; ++it)
		subtitles.Add(UCSCoreLibrary::FrySubtitle(it.Data(), it.ID(), iCurrentSettings));

	ReconstructSubtitlesEvent.Broadcast(subtitles, iCurrentSettings);
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

void UCSS_SubtitleGISS::ClearCaptions()
{
	TArray<FTimerHandle> flushedHandles;
	TArray<int32> flushedIDs;
	iCurrentCaptions.Flush(flushedHandles, flushedIDs);

	if (flushedIDs.IsEmpty())
		return;

	iManageRemoval(flushedHandles, flushedIDs);
	
	iReconstructCaptions();
}

void UCSS_SubtitleGISS::PauseCaptions()
{
	if (!uTimerManager)
		return;

	iCaptionsArePaused = true;

	for (FTimerHandle const& handle : iCurrentCaptions.Handles())
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
	
	iCaptionsArePaused = false;

	for (FTimerHandle const& handle : iCurrentCaptions.Handles())
		uTimerManager->UnPauseTimer(handle);

	for (FTimerHandle const& handle : iQueuedCaptions.Handles())
		uTimerManager->UnPauseTimer(handle);
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
		iBroadcastCaption(caption, tNow(), id);
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
		const FFullCaption fullCap = FFullCaption(caption, source, sound, dtDisplay);
		const int32 id = iIDManager.New();
		newIDs.Add(id);

		if (fullCap.StartDelay == 0)
		{
			//ensure broadcast, even with start time of zero
			iBroadcastCaption(fullCap, tNow(), id);
		}
		else
		{
			FTimerHandle& handle = iQueuedCaptions.Add(fullCap, id);
			iSetTimer(&UCSS_SubtitleGISS::iOnCaptionTriggered, handle, id, caption.StartDelay);
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
		newIDs.Add(id);

		if (caption.StartDelay == 0)
		{
			//ensure broadcast, even with start time of zero
			iBroadcastCaption(caption, tNow(), id);
		}
		else
		{
			FTimerHandle& handle = iQueuedCaptions.Add(caption, id);
			iSetTimer(&UCSS_SubtitleGISS::iOnCaptionTriggered, handle, id, caption.StartDelay);
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
	for (const int32 id : ids)
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
	iBroadcastCaption(iQueuedCaptions.Consume(id), tNow(), id);
}

void UCSS_SubtitleGISS::iBroadcastCaption(FFullCaption const& caption, const float tNow, const int32 id)
{
	if (bCaptionsAsSubtitles)
	{
		const FFullSubtitle subtitle = FFullSubtitle(caption, UCSProjectSettingFunctions::GetSpeakerNameForCaptions());
		iBroadcastSubtitle(subtitle, tNow, id);
		return;
	}

	if (!iSourcesManager.GetSources().Contains(caption.SoundID.Source))
		return;//If the source isn't registered (or is excluded due to SourceOverride) we don't want to broadcast.

	const bool isPermanent = caption.DisplayDuration < 0;

	const float dtDisplay = FMath::Abs(caption.DisplayDuration);

	if (iTryUpdateCaptionDuration(caption.SoundID, dtDisplay, isPermanent))
		return;

	FTimerHandle& handle = iCurrentCaptions.Add(caption, id, isPermanent);

	if (!isPermanent)
	{
		if (dtDisplay < iCurrentSettings->MinimumCaptionTime)
		{
			iSetTimer(&UCSS_SubtitleGISS::iDestroyCaption, handle, id, iCurrentSettings->MinimumCaptionTime);
		}
		else
		{
			iSetTimer(&UCSS_SubtitleGISS::iDestroyCaption, handle, id, dtDisplay);
		}
	}

	if (!iCurrentSettings->bShowCaptions)
		return;//We will track captions without displaying them.

	ConstructCaptionEvent.Broadcast(FCrispCaption(caption, id));
}

void UCSS_SubtitleGISS::iDestroyCaption(const int32 id)
{
	iCurrentCaptions.Remove(id);
	iManageRemoval(id);
	DestructCaptionEvent.Broadcast(id);
}

void UCSS_SubtitleGISS::iReconstructCaptions() const
{
	TArray<FCrispCaption> captions;
	captions.Reserve(iCurrentCaptions.Num());

	for (auto it = iCurrentCaptions.GetIterator(); it; ++it)
		captions.Add(FCrispCaption(it.Data(), it.ID()));

	ReconstructCaptionsEvent.Broadcast(captions, iCurrentSettings);
}

bool UCSS_SubtitleGISS::iTryUpdateCaptionDuration(FCSSoundID const& soundID, const float dtDisplay, const bool isPermanent)
{
	for (auto it = iCurrentCaptions.GetVolatileIterator(); it; ++it)
	{
		if (it.Data().SoundID == soundID)
		{
			if (isPermanent && uTimerManager)
			{
				uTimerManager->ClearTimer(it.Handle());
			}
			else
			{
				iSetTimer(&UCSS_SubtitleGISS::iDestroyCaption, it.Handle(), it.ID(), dtDisplay);
			}

			return true;
		}
	}

	return false;
}

#pragma endregion

#pragma region DATA ACCESS

float UCSS_SubtitleGISS::GetBusyDuration() const
{
	return iSubtitleBroadcastData.dtBusy(tNow());
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

bool UCSS_SubtitleGISS::RegisterAndTrackSound(FCSSoundID const& soundID, FVector const& location, ULocalPlayer const* player)
{
	if (!RegisterSource(soundID.Source))
		return false;
	else
		return TrackSound(soundID, location, player);
}

bool UCSS_SubtitleGISS::TrackSound(FCSSoundID const& soundID, FVector const& location, ULocalPlayer const* player)
{
	if (!iSourcesManager.TrackSound(soundID, location, player))
		return false;
	
	LocationUpdated.Broadcast();
	return true;
}

bool UCSS_SubtitleGISS::GetSoundLocation(FCSSoundID const& soundID,  FVector& location, ULocalPlayer const* player) const
{
	return iSourcesManager.GetSoundData(soundID, location, player);
}

TArray<FVector> UCSS_SubtitleGISS::GetSoundLocationDump(TArray<FCSSoundID>& soundIDs, ULocalPlayer const* player) const
{
	soundIDs.Empty();//Ensures sync with positions; we use append later.

	return iSourcesManager.GetSoundDataDump(soundIDs, player);
}

void UCSS_SubtitleGISS::StopTrackingSound(FCSSoundID const& soundID, ULocalPlayer const* player)
{
	iSourcesManager.StopTrackingSound(soundID, player);
}

bool UCSS_SubtitleGISS::UnregisterSource(const FName source, const bool removeSubtitles, const bool removeCaptions)
{
	if (!iSourcesManager.RemoveSource(source))
		return false;

	RemoveQueuedSubtitlesBySource(source);
	RemoveQueuedCaptionsBySource(source);

	if (removeSubtitles)
		iRemoveCurrentSubtitlesBySource(source);

	if (removeCaptions)
		iRemoveCurrentCaptionsBySource(source);

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

void UCSS_SubtitleGISS::iRemoveCurrentSubtitlesBySource(const FName source)
{
	TArray<FTimerHandle> removedHandles = TArray<FTimerHandle>();
	TArray<int32> removedIDs = TArray<int32>();

	const bool hadPermanent = HasPermanentSubtitle();

	iCurrentSubtitles.RemoveBySource(source, removedHandles, removedIDs);

	if (removedHandles.IsEmpty())
		return;

	iManageRemoval(removedHandles, removedIDs);

	for (const int32 id : removedIDs)
		DestructCaptionEvent.Broadcast(id);

	if (hadPermanent && !HasPermanentSubtitle())
	{
		PermanentSubtitlesRemoved.Broadcast();
	}
}

void UCSS_SubtitleGISS::iRemoveCurrentCaptionsBySource(const FName source)
{
	TArray<FTimerHandle> removedHandles = TArray<FTimerHandle>();
	TArray<int32> removedIDs = TArray<int32>();

	for (auto it = iCurrentCaptions.GetVolatileIterator(); it; ++it)
	{
		if (it.Data().SoundID.Source == source)
		{
			removedHandles.Add(it.Handle());
			removedIDs.Add(it.ID());
			it.RemoveCurrent();
		}
	}

	if (removedHandles.IsEmpty())
		return;

	iManageRemoval(removedHandles, removedIDs);
	iReconstructCaptions();
}

#pragma endregion

#pragma region INDICATORS

void UCSS_SubtitleGISS::UpdateIndicatorData(ULocalPlayer const* player)
{
	iSourcesManager.CallCalculate(player);
}

void UCSS_SubtitleGISS::UnregisterIndicator(FCSSoundID const& soundID, ULocalPlayer const* player, UObject* widget)
{
	iSourcesManager.UnregisterIndicator(soundID, player, widget);
}

#pragma endregion

#pragma region SETTINGS
void UCSS_SubtitleGISS::RecalculateLayout()//TODO
{
	if (UGameViewportClient* viewportClient = GetGameInstance()->GetGameViewportClient())
	{
		iCurrentSettings->RecalculateLayout(viewportClient);
	}
}

void UCSS_SubtitleGISS::SetSettings(UCSUserSettings* settings)
{
	if (!settings || iCurrentSettings == settings)
		return;//We don't want to assign nullptr. Neither do we want to force UI updates when nothing has changed.

	iCurrentSettings = settings;

	iReconstructSubtitles();
}

#pragma endregion