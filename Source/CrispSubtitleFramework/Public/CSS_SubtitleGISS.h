// Copyright Crisp Clover.

#pragma once

#include "Subsystems/GameInstanceSubsystem.h"
#include "CSProjectSettingFunctions.h"
#include "CSUserSettings.h"
#include "CSSourcesManager.h"//TODO: move?
#include "Engine/ObjectLibrary.h"//TODO: move once inlines are gone
#include "Engine/AssetManager.h"//TODO: move?
#include "CSS_SubtitleGISS.generated.h"

class UCSCustomDataManager;
class UObjectLibrary;

template<typename DataElement>
struct TCSCurrentData;

#pragma region STRUCTS

//Manages IDs
struct FCSIDManager
{
	int32 New()
	{
		const int32 id = iLowestFreeID;
		iIDs.Add(id);

		while (iIDs.Contains(iLowestFreeID))
			iLowestFreeID++;

		return id;
	};

	void Delete(const int32 ID)
	{
		iIDs.Remove(ID);
		if (ID < iLowestFreeID)
			iLowestFreeID = ID;
	};

	void Clear()
	{
		iIDs.Empty();
		iLowestFreeID = 0;
	};

	inline void Shrink()
	{ iIDs.Compact(); };

	inline bool Contains(const int32 id) const
	{ return iIDs.Contains(id); };

private:
	TSet<int32> iIDs = TSet<int32>();
	int32 iLowestFreeID = 0;
};

//Manages FTimerHandle creation and tracks the data required to identify and construct subtitles/captions.
template<typename DataElement>
struct TCSTimedData
{
	inline TArray<DataElement> const& Elements() const
	{ return iDataElements; };

	inline TArray<FTimerHandle> const& Handles() const
	{ return iTimerHandles; };

	inline TArray<int32> const& IDs() const
	{ return iIDs; };

	inline int32 Num() const
	{ return iDataElements.Num(); };

	//Stores an Element alongside its ID; Creates and returns a new TimerHandle (by ref).
	inline FTimerHandle& Add(const DataElement data, const int32 id)
	{
		iDataElements.Add(data);
		iIDs.Add(id);
		return iTimerHandles.AddDefaulted_GetRef();
	};

	DataElement Consume(const int32 id)
	{
		for (int32 i = 0; i < iIDs.Num(); i++)
		{
			if (iIDs[i] == id)
			{
				const DataElement element = iDataElements[i];
				uRemoveAt(i);
				return element;
			}
		}

		return DataElement();
	};

	FTimerHandle Remove(int32 id)
	{
		for (int32 i = iIDs.Num() - 1; i >= 0; i--)
		{
			if (iIDs[i] == id)
			{
				FTimerHandle handle = iTimerHandles[i];
				uRemoveAt(i);
				return handle;
			}
		}

		return FTimerHandle();
	};

	inline void Flush(TArray<FTimerHandle>& flushedHandles, TArray<int32>& flushedIDs)
	{
		flushedHandles = iTimerHandles;
		flushedIDs = iIDs;
		iDataElements.Empty();
		iTimerHandles.Empty();
		iIDs.Empty();
	};

	inline void Reserve(const int32 n)
	{
		iDataElements.Reserve(n);
		iTimerHandles.Reserve(n);
		iIDs.Reserve(n);
	};

	inline void Shrink()
	{
		iDataElements.Shrink();
		iTimerHandles.Shrink();
		iIDs.Shrink();
	};

private:
	inline void uRemoveAt(const int32 index)
	{
		iDataElements.RemoveAt(index, 1);
		iTimerHandles.RemoveAt(index, 1);
		iIDs.RemoveAt(index, 1);
	};

	TArray<DataElement> iDataElements = TArray<DataElement>();
	TArray<FTimerHandle> iTimerHandles = TArray<FTimerHandle>();
	TArray<int32> iIDs = TArray<int32>();
	
	friend struct TCSCurrentData<DataElement>;

	// --- ITERATOR --- //
public:
	struct Iterator
	{
		Iterator(TCSTimedData& timedData, const int32 startIndex = 0)
			: iData(timedData)
			, iIndex(startIndex)
		{};
		
		inline Iterator& operator++()
		{
			iIndex++;
			return *this;
		};
		
		inline Iterator& operator--()
		{
			iIndex--;
			return *this;
		}
		
		FORCEINLINE explicit operator bool() const
		{ return iData.iDataElements.IsValidIndex(iIndex); };
		
		inline void Reset()
		{ iIndex = 0; };

		FORCEINLINE DataElement Data() const
		{ return iData.iDataElements[iIndex]; };

		FORCEINLINE FTimerHandle& Handle() const
		{ return iData.iTimerHandles[iIndex]; };

		FORCEINLINE int32 ID() const
		{ return iData.iIDs[iIndex]; };
		
		void RemoveCurrent()
		{
			iData.uRemoveAt(iIndex);
			iIndex--;
		};

	private:
		TCSTimedData& iData;
		int32 iIndex = 0;
	};

	inline Iterator GetIterator()
	{ return Iterator(*this); };
};

//Bundles the data of subtitles/captions, and their timer handles.
template<typename DataElement>
struct TCSCurrentData
{
	inline TArray<DataElement> const& Get() const
	{ return iTimedData.Elements(); };

	inline TArray<FTimerHandle> const& Handles() const
	{ return iTimedData.Handles(); };

	inline int32 Num() const
	{ return iTimedData.Num(); };

	//Stores an Element alongside its ID, Creates and returns a new TimerHandle (by ref).
	inline FTimerHandle& Add(const DataElement data, const int32 id, const bool isPermanent)
	{ 
		iArePermanent.Add(isPermanent);
		return iTimedData.Add(data, id);
	};
	
	void Remove(const int32 id)
	{
		for (int32 i = Num() - 1; i >= 0; i--)
		{
			if (iTimedData.iIDs[i] == id)
			{
				uRemoveAt(i);
				return;
			}
		}
	};

	inline void Flush(TArray<FTimerHandle>& flushedHandles, TArray<int32>& flushedIDs)
	{
		iArePermanent.Empty();
		iTimedData.Flush(flushedHandles, flushedIDs);
	};

	inline void Reserve(const int32 n)
	{
		iArePermanent.Reserve(n);
		iTimedData.Reserve(n);
	};

	inline void Shrink()
	{
		iArePermanent.Shrink();
		iTimedData.Shrink()
	};

private:
	inline void uRemoveAt(const int32 index)
	{
		iArePermanent.RemoveAt(index, 1);
		iTimedData.uRemoveAt(index);
	};

	TCSTimedData<DataElement> iTimedData = TCSTimedData<DataElement>();
	TArray<bool> iArePermanent = TArray<bool>();

	friend struct FCSCurrentSubtitleData;

	// --- ITERATOR --- //
public:

	struct Iterator
	{
		Iterator(TCSTimedData<DataElement> const& timedData, const int32 startIndex = 0)
			: iData(timedData)
			, iIndex(startIndex)
		{};
		
		inline Iterator& operator++()
		{
			iIndex++;
			return *this;
		};
		
		inline Iterator& operator--()
		{
			iIndex--;
			return *this;
		}
		
		FORCEINLINE explicit operator bool() const
		{ return iData.iDataElements.IsValidIndex(iIndex); };
		
		inline void Reset()
		{ iIndex = 0; };

		FORCEINLINE DataElement Data() const
		{ return iData.iDataElements[iIndex]; };

		FORCEINLINE FTimerHandle Handle() const
		{ return iData.iTimerHandles[iIndex]; };

		FORCEINLINE int32 ID() const
		{ return iData.iIDs[iIndex]; };

		FORCEINLINE int32 IsPermanent() const
		{ return iData.iArePermanent[iIndex]; };

		FORCEINLINE int32 xCurrent() const
		{ return iIndex; };

	private:
		TCSTimedData<DataElement> const& iData;
		int32 iIndex = 0;
	};

	inline Iterator GetIterator() const
	{ return Iterator(iTimedData); };
};

//Bundles the data of subtitles and their number of lines. Ensures maximum number of subtitles and lines are not exceeded.
struct FCSCurrentSubtitleData
{
	FCSCurrentSubtitleData()
	{ iCurrentData.Reserve(UCSProjectSettingFunctions::GetMaxSubtitles()); };

	inline TArray<FFullSubtitle> const& Get() const
	{ return iCurrentData.Get(); };

	inline TArray<FTimerHandle> const& Handles() const
	{ return iCurrentData.Handles(); };

	inline int32 Num() const
	{ return iCurrentData.Num(); };

	FTimerHandle& Add(FFullSubtitle const& subtitle, const int32 id, const bool isPermanent, TArray<FTimerHandle>& removedHandles, TArray<int32>& removedIDs)
	{
		icLines += subtitle.Lines.Num();

		while (icLines > UCSProjectSettingFunctions::GetMaxSubtitleLines() || iCurrentData.Num() == UCSProjectSettingFunctions::GetMaxSubtitles())
		{
			FTimerHandle kickedHandle;
			int32 kickedID;
			Kick(kickedHandle, kickedID);
			removedHandles.Add(kickedHandle);
			removedIDs.Add(kickedID);
		}

		return iCurrentData.Add(subtitle, id, isPermanent);
	};

	void Remove(const int32 id)
	{
		for (auto it = iCurrentData.GetIterator(); it; ++it)
		{
			if (it.ID() == id)
			{
				icLines -= it.Data().Lines.Num();
				iCurrentData.uRemoveAt(it.xCurrent());
				return;
			}
		}
	}

	void RemoveBySource(const FName source, TArray<FTimerHandle>& removedHandles, TArray<int32>& removedIDs)
	{
		for (auto it = iCurrentData.GetIterator(); it; ++it)
		{
			if (it.Data().Source == source)
			{
				icLines -= it.Data().Lines.Num();
				removedHandles.Add(it.Handle());
				removedIDs.Add(it.ID());
				iCurrentData.uRemoveAt(it.xCurrent());
			}
		}
	}

	//Copies the TimerHandle and ID at index 0, then removes all data at index 0.
	inline void Kick(FTimerHandle& handle, int32& id)
	{
		icLines -= iCurrentData.iTimedData.Elements()[0].Lines.Num();
		handle = iCurrentData.iTimedData.Handles()[0];
		id = iCurrentData.iTimedData.IDs()[0];
		iCurrentData.uRemoveAt(0);
	};

	inline void Flush(TArray<FTimerHandle>& flushedHandles, TArray<int32>& flushedIDs)
	{
		icLines = 0;
		iCurrentData.Flush(flushedHandles, flushedIDs);
	};

	inline TCSCurrentData<FFullSubtitle>::Iterator GetIterator() const
	{ return iCurrentData.GetIterator(); };

private:
	TCSCurrentData<FFullSubtitle> iCurrentData = TCSCurrentData<FFullSubtitle>();
	int32 icLines = 0;
};

//Bundle to calculate flicker protection delta-times.
struct FCSFlickerProtectionData
{
	inline float tLastSet() const
	{ return itLastChanged; };

	inline float dtMissing(float tNow, float dtGap) const
	{
		if (inFrame == GFrameNumber)
			return 0.f;
		else
			return FMath::Min(0, dtGap - (tNow - itLastChanged));
	};

	inline void Set(float tNow)
	{
		itLastChanged = tNow;
		inFrame = GFrameNumber;
	};

private:
	float itLastChanged = 0;
	uint32 inFrame = 0;
};

//Bundle to control broadcasts.
struct FCSBroadcastingData
{
	FCSBroadcastingData()
		: iProtectConstruct()
		, iProtectDestruct()
		, iIsDelayingDestruction(false)
		, iIsPaused(false)
	{};

	inline void LogConstruction(const float tNow)
	{ iProtectDestruct.Set(tNow); };

	inline void LogDestruction(const float tNow)
	{ iProtectConstruct.Set(tNow); };

	inline void LogPaused()
	{ iIsPaused = true; };

	inline void LogUnpaused()
	{ iIsPaused = false; };

	inline bool LogDelay(const bool isDelaying)
	{ return iIsDelayingDestruction = isDelaying; };

	inline bool IsDelayingDestruction() const
	{ return iIsDelayingDestruction; };

	inline bool IsPaused() const
	{ return iIsPaused; };

	inline float tLastConstruction() const
	{ return iProtectDestruct.tLastSet(); };

	//Returns the time to wait before we're allowed to add an element.
	inline float dtFlickerProtectConstruct(const float tNow, const float dtGap) const
	{ return iProtectConstruct.dtMissing(tNow, dtGap); };

	//Returns the time to wait before we're allowed to remove an element.
	inline float dtFlickerProtectDestruct(const float tNow, const float dtGap) const
	{ return iProtectDestruct.dtMissing(tNow, dtGap); };

private:
	FCSFlickerProtectionData iProtectConstruct;
	FCSFlickerProtectionData iProtectDestruct;

	bool iIsDelayingDestruction;
	bool iIsPaused;
};

//Bundle to control subtitle broadcasts.
struct FCSSubtitleBroadcastingData : FCSBroadcastingData
{
	FCSSubtitleBroadcastingData()
		: idtBusy(0)
	{};

	inline float dtBusy(const float tNow) const
	{ return FMath::Max(0.f, idtBusy + tLastConstruction() - tNow); };

	inline void LogBroadcast(const float tNow, const float dtBusy)
	{
		LogConstruction(tNow);
		idtBusy = dtBusy;
	};

private:
	float idtBusy;
};

#pragma endregion

#pragma region DELEGATES

//Delegate for the subtitle trigger.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSubtitleTrigger, FCrispSubtitle const&, Subtitle);

//Delegate for the subtitle trigger.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FReconstructSubtitlesTrigger, TArray<FCrispSubtitle> const&, Subtitles);

//Delegate for the caption trigger.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCaptionTrigger, FCrispCaption const&, Caption);

//Delegate for the subtitle trigger.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FReconstructCaptionsTrigger, TArray<FCrispCaption> const&, Captions);

//Delegate for triggering destruction.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDestructTrigger, const int32, Index);

#pragma endregion

/**
 *
 */
UCLASS()
class CRISPSUBTITLEFRAMEWORK_API UCSS_SubtitleGISS : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

private:
	inline void iManageRemoval(FTimerHandle& handle, const int32 id)
	{
		iIDManager.Delete(id);

		if (iTimerManager)
			iTimerManager->ClearTimer(handle);
	}

	inline void iManageRemoval(TArray<FTimerHandle>& handles, TArray<int32> const& ids)
	{
		for (const int32 id : ids)
			iIDManager.Delete(id);

		if (iTimerManager)
			for (FTimerHandle& handle : handles)
				iTimerManager->ClearTimer(handle);
	}

	FTimerManager* iTimerManager = nullptr;
	FCSIDManager iIDManager = FCSIDManager();

#pragma region SUBTITLES
public:
	//Called when a subtitle should begin to be displayed.
	UPROPERTY(BlueprintAssignable, Category = "CrispSubtitles|Events")
		FSubtitleTrigger ConstructSubtitleEvent;
	
	//Called when a subtitle should stop being displayed.
	UPROPERTY(BlueprintAssignable, Category = "CrispSubtitles|Events")
		FDestructTrigger DestroySubtitleEvent;
	
	/**
	 * Called when the subtitle UI needs to be reconstructed.
	 * (e.g. when the settings have changed)
	 */
	UPROPERTY(BlueprintAssignable, Category = "CrispSubtitles|Events")
		FReconstructSubtitlesTrigger ReconstructSubtitlesEvent;

	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Subtitles")
		FORCEINLINE TArray<FFullSubtitle> const& GetCurrentSubtitles() const
			{ return iCurrentSubtitles.Get(); };

	//Removes the oldest subtitle from the UI.
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Subtitles")
		void KickSubtitle();

	//Removes all current subtitles from the UI.
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Subtitles")
		void ClearSubtitles();
	
	//Pauses all timers for all subtitles controlled by the subsystem.
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Subtitles")
		void PauseSubtitles();
	
	//Resumes all timers for all subtitles controlled by the subsystem.
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Subtitles")
		void UnpauseSubtitles();

	/**
	 * Queues a subtitle with the subsystem.
	 * @param Subtitle The subtitle.
	 * @return The ID of the subtitle.
	 */
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Queue")
		int32 QueueSubtitle(FFullSubtitle const& Subtitle);
	
	/**
	 * Queues an array of raw subtitles with the subsystem.
	 * @param Subtitles The raw subtitles.
	 * @param Speaker The speaker.
	 * @param SpeakerID The speaker's ID.
	 * @param SourceName The ID of the source used for registration.
	 * @return The IDs of the subtitles.
	 */
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Queue")
		TArray<int32> QueueRawSubtitles(TArray<FRawSubtitle> const& Subtitles, FText const& Speaker, const FName SpeakerID, const FName SourceName);
	
	/**
	 * Queues an array of group subtitles with the subsystem.
	 * @param Subtitles The group subtitles.
	 * @param SourceName The ID of the source used for registration.
	 * @return The IDs of the subtitles.
	 */
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Queue")
		TArray<int32> QueueGroupSubtitles(TArray<FGroupSubtitle> const& Subtitles, const FName SourceName);
	
	/**
	 * Queues an array of full subtitles with the subsystem.
	 * @param Subtitles The full subtitles.
	 * @return The IDs of the subtitles.
	 */
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Queue")
		TArray<int32> QueueFullSubtitles(TArray<FFullSubtitle> const& Subtitles);
		
	/**
	 * Removes a queued subtitle.
	 * @param ID The ID of the subtitle.
	 */
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Subtitles")
		void RemoveQueuedSubtitle(const int32 ID);
	
	/**
	 * Removes the queued subtitles with the specified IDs.
	 * @param IDs The IDs of the subtitles.
	 */
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Subtitles")
		void RemoveQueuedSubtitles(TArray<int32> const& IDs);
	
	/**
	 * Removes all queued subtitles sharing the specified source.
	 * @param SourceName The name of the source when it was registered with the subsystem.
	 */
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Subtitles")
		void RemoveQueuedSubtitlesBySource(const FName SourceName);
	
private:
	void iOnSubtitleTriggered(const int32 id);
	void uBroadcastSubtitle(FFullSubtitle const& subtitle, const float tNow, const int32 id);
	void iBroadcastSubtitle(FFullSubtitle const& subtitle, const float tNow, const int32 id);
	void iDelaySubtitle(FFullSubtitle const& subtitle, const int32 id, const float dtMissing);
	void iDelayedBroadcastSubtitles();
	void iDestroySubtitle(const int32 id);
	void iDelayedDestroySubtitles();
	void uReconstructSubtitles() const;

	FCSCurrentSubtitleData iCurrentSubtitles = FCSCurrentSubtitleData();
	TMap<int32, FFullSubtitle> iDelayedSubtitles = TMap<int32, FFullSubtitle>();
	TCSTimedData<FFullSubtitle> iQueuedSubtitles = TCSTimedData<FFullSubtitle>();
	FCSSubtitleBroadcastingData iSubtitleBroadcastData = FCSSubtitleBroadcastingData();

#pragma endregion

#pragma region CAPTIONS
public:
	//By default this is set to bDisplayCaptionsAsSubtitles (defined in the project settings). Exposed here to be changed at runtime if needed.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CrispSubtitles|Captions")
		bool bCaptionsAsSubtitles = UCSProjectSettingFunctions::GetDisplayCaptionsAsSubtitles();

	//Called when a caption should begin to be displayed.
	UPROPERTY(BlueprintAssignable, Category = "CrispSubtitles|Events")
		FCaptionTrigger ConstructCaptionEvent;
	
	//Called when a caption should stop being displayed.
	UPROPERTY(BlueprintAssignable, Category = "CrispSubtitles|Events")
		FDestructTrigger DestructCaptionEvent;
	
	/**
	 * Called when the caption UI needs to be reconstructed.
	 * (e.g. when the settings have changed)
	 */
	UPROPERTY(BlueprintAssignable, Category = "CrispSubtitles|Events")
		FReconstructCaptionsTrigger ReconstructCaptionsEvent;

	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Captions")
		FORCEINLINE TArray<FFullCaption> const& GetCurrentCaptions() const
			{ return iCurrentCaptions.Get(); };
	
	//Pauses all timers for all captions controlled by the subsystem.
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Captions")
		void PauseCaptions();
	
	//Resumes all timers for all captions controlled by the subsystem.
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Captions")
		void UnpauseCaptions();
	
	/**
	 * Queues a caption with the subsystem.
	 * @param Caption The caption.
	 * @return The ID of the caption.
	 */
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Queue")
		int32 QueueCaption(FFullCaption const& Caption);
	
	/**
	 * Queues an array of sound captions with the subsystem.
	 * @param Captions The sound captions.
	 * @param SourceName The ID of the source used for registration.
	 * @param SoundName The ID of the sound.
	 * @param DisplayDuration The duration that each of the captions should be displayed for.
	 * @return The IDs of the captions.
	 */	
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Queue")
		TArray<int32> QueueCaptions(TArray<FSoundCaption> const& Captions, const FName SourceName, const FName SoundName, const float DisplayDuration);

	/**
	 * Queues an array of full captions with the subsystem.
	 * @param Captions The full captions.
	 * @return The IDs of the captions.
	 */
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Queue")
		TArray<int32> QueueFullCaptions(TArray<FFullCaption> const& Captions);

	/**
	 * Removes a queued caption.
	 * @param ID The ID of the caption.
	 */
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Subtitles")
		void RemoveQueuedCaption(const int32 IDs);

	/**
	 * Removes the queued captions with the specified IDs.
	 * @param ID The IDs of the captions.
	 */
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Subtitles")
		void RemoveQueuedCaptions(TArray<int32> const& IDs);

	/**
	 * Removes all queued captions sharing the specified source.
	 * @param SourceName The name of the source when it was registered with the subsystem.
	 */
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Subtitles")
		void RemoveQueuedCaptionsBySource(const FName SourceName);

private:
	void iOnCaptionTriggered(const int32 id);
	void uBroadcastCaption(FFullCaption const& caption, const float tNow, const int32 id);
	void iBroadcastCaption(FFullCaption const& caption, const float tNow, const int32 id);
	void iDelayCaption(FFullCaption const& caption, const int32 id, const float dtMissing);
	void iDelayedBroadcastCaptions();
	void iDestroyCaption(const int32 id);
	void iDelayedDestroyCaptions();
	void uReconstructCaptions() const;

	TCSCurrentData<FFullCaption> iCurrentCaptions = TCSCurrentData<FFullCaption>();
	TMap<int32, FFullCaption> iDelayedCaptions = TMap<int32, FFullCaption>();
	TCSTimedData<FFullCaption> iQueuedCaptions = TCSTimedData<FFullCaption>();
	FCSBroadcastingData iCaptionBroadcastData = FCSBroadcastingData();

#pragma endregion

#pragma region DATA ACCESS
	// --- DATA --- // TODO: more?
public:
	//Returns the approximated time the user will remain busy with reading the currently displayed subtitles.
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Data")
		float GetBusyDuration() const;

	//Returns whether a subtitle without infinite duration exists.
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Data")
		bool HasPermanentSubtitle() const;

	//The object that will manage the custom data for subtitles.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CrispSubtitles|CustomData")
		UCSCustomDataManager* CustomData = nullptr;

#pragma endregion

#pragma region SOURCE MANAGEMENT
public:
	/**
	 * Registers a sound source with this subsystem without including any location data.
	 * @param SourceName The name to use for identification.
	 * @return false if SourceName is already registered.
	 */
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Sources")
		bool RegisterSource(const FName SourceName);

	/**
	 * Registers a sound source with this subsystem. Assigns a unique name to the source.
	 * @param Source The actor to add as a source.
	 * @param SourceName The unique name of the source.
	 * @return false if the source is already registered.
	 */
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Sources")
		bool RegisterAutoNamedSource(AActor const* Source, FName& SourceName);

	/**
	 * Registers a sound source with this subsystem for tracking via direction indicators.
	 * @param SoundID The ID of the sound.
	 * @param SourceLocation The world position to use as the source's location.
	 * @param Player The player to register the sound source with. If left blank, the source will register with all players.
	 * @return false if SourceName is already registered.
	 */
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Sources")
		bool RegisterAndTrackSound3D(FCSSoundID const& SoundID, FVector const& SourceLocation, ULocalPlayer const* Player = nullptr);

	/**
	 * Registers a 2D sound source with this subsystem.
	 * @param SoundID The ID of the sound.
	 * @param SourcePosition The screen space position to use as the source's location.
	 * @param Player The player to register the sound source with. If left blank, the source will register with all players.
	 * @return false if SourceName is already registered.
	 */
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Sources")
		bool RegisterAndTrackSound2D(FCSSoundID const& SoundID, FVector2D const& SourcePosition, ULocalPlayer const* Player = nullptr);

	/**
	 * Starts tracking a registered sound source for direction indicators.
	 * @param SoundID The ID used at registration.
	 * @param SourceLocation The world position to use as the source's location.
	 * @param Player The player to change the sound location for. If left blank, the location will update for all players.
	 * @return false if source hasn't been registered yet.
	 */
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Sources")
		bool TrackSound3D(FCSSoundID const& SoundID, FVector const& SourceLocation, ULocalPlayer const* Player = nullptr);

	/**
	 * Starts tracking a registered sound source for direction indicators.
	 * @param SoundID The ID used at registration.
	 * @param SourcePosition The screen position to use.
	 * @param Player The player to change the sound position for. If left blank, the position will update for all players.
	 * @return false if source hasn't been registered yet.
	 */
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Sources")
		bool TrackSound2D(FCSSoundID const& SoundID, FVector2D const& SourcePosition, ULocalPlayer const* Player = nullptr);

	/**
	 * @param SoundID The ID used at registration.
	 * @param SourceLocation The currently registered position of the source.
	 * @param Player TODO
	 * @return false if the source isn't tracked/registered.
	 */
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Sources")
		bool GetSoundLocation(FCSSoundID const& SoundID, FVector& Location, ULocalPlayer const* Player = nullptr) const;

	/**
	 * Removes a sound from the list of tracked sounds.
	 * @param SoundID The ID used at registration.
	 * @param Player The player to remove the sounds data from. If left blank, the data will be removed from all players.
	 */
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Sources")
		void StopTrackingSound(FCSSoundID const& SoundID, ULocalPlayer const* Player = nullptr);

	/**
	 * Unregisters a source from this subsystem.
	 * @param SourceName The name used for identification.
	 * @return true if any sources have been removed.
	 */
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Sources")
		bool UnregisterSource(const FName SourceName);
	
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Sources")
		void EmptySources();

	//Removes all sources from the subsystem. TODO
	//UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Sources")
		//void ClearSources();

	/**
	 * @param SoundID The ID of the sound.
	 * @param Player The player to check for the tracked sound. If left blank, the function will return true if any player tracks the source.
	 * @return Whether the given source is being tracked.
	 */
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Sources")
		bool IsSoundTracked(FCSSoundID const& soundID, ULocalPlayer const* Player = nullptr) const;

	//Retrieves the names of all registered sound sources.
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Sources")
		TSet<FName> GetSourceNames() const;

	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Sources")
		bool HasSourcesOverride() const;

	/**
	 * TODO
	 */
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Sources")
		void SetSourcesOverride(TSet<FName> const& SourcesOverride);

	/**
	 * TODO
	 */
	//UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Sources")
		//TSet<FName> const& GetSourcesOverride() const;

	/**
	 * TODO
	 */
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Sources")
		void ClearSourcesOverride();

private:
	FCSSourcesManager iSourcesManager = FCSSourcesManager();

#pragma endregion

#pragma region INDICATORS
public:

	/**
	 * Forces recalculation of the indicator data. Doesn't need to be called by default.
	 * If you want to run calculations on a timer, use this and disable bCalculateIndicatorsOnTick in the project settings.
	 */
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Tick")
		void UpdateIndicatorData(ULocalPlayer const* Player);

	/**
	 * Registers an indicator widget with this subsystem.
	 * @param Args The ID of the sound and a reference to the widget data pointer.
	 * @param Player The player the indicator widget belongs to.
	 * @return The delegates to subscribe to for updates, can be null.
	 */
	CSIndicatorDelegates* RegisterIndicator(FCSRegisterArgs Args, ULocalPlayer const* Player = nullptr);
	
	/**
	 * Removes an indicator from the list of registered indicators.
	 * @param SoundID The ID of the sound.
	 * @param Player The player the indicator widget belongs to.
	 * @param Widget The widget to unregister.
	 */
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Tick")
	void UnregisterIndicator(FCSSoundID const& SoundID, ULocalPlayer const* Player, UObject* Widget);

#pragma endregion

//TODO: remove inlines, make public
#pragma region SAVING
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|TODO")
		inline void ResumeFromSaveDump(TArray<FFullSubtitle> const& current, TArray<FFullSubtitle> const& queued)//TODO
	{
		TArray<FCrispSubtitle> subtitles;
		//TODO: restore sources
		TArray<FTimerHandle> removedHandles = TArray<FTimerHandle>();
		TArray<int32> removedIDs = TArray<int32>();
		const float now = itNow();
		for (int32 i = 0; i < current.Num(); i++)
		{
			int32 id = iIDManager.New();
			FTimerHandle& handle = iCurrentSubtitles.Add(current[i], id, false/*TODO*/, removedHandles, removedIDs);
			iSetTimer(&UCSS_SubtitleGISS::iDestroySubtitle, handle, id, current[i].ReadDuration);

			subtitles.Add(UCSLibrary::FrySubtitle(current[i], id, iCurrentSettings));
		}
		//TODO: clear removed handles

		ReconstructSubtitlesEvent.Broadcast(subtitles);
		iSubtitleBroadcastData.LogBroadcast(now, current.Last().ReadDuration);

		QueueFullSubtitles(queued);
	};

	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|TODO")
		inline void SaveDump(TArray<FFullSubtitle>& current, TArray<FFullSubtitle>& queued)//TODO
	{
		//TODO: dump sources?
		FTimerManager& tm = GetGameInstance()->GetTimerManager();
		TArray<FFullSubtitle> delayed;
		iDelayedSubtitles.GenerateValueArray(delayed);

		current = GetCurrentSubtitles();
		current.Append(delayed);

		TArray<FTimerHandle> const& cHandles = iCurrentSubtitles.Handles();
		for (int32 i = 0; i < cHandles.Num(); i++)
			current[i].ReadDuration = tm.GetTimerRemaining(cHandles[i]);

		for (auto it = iQueuedSubtitles.GetIterator(); it; ++it)//override each subtitle's StartDelay with TimerRemaining
			queued.Add(FFullSubtitle(it.Data(), tm.GetTimerRemaining(it.Handle())));
	};
	
#pragma endregion

#pragma region SETTINGS
public:
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Settings")
		FORCEINLINE UCSUserSettings* GetCurrentSettings() const
			{ return iCurrentSettings; };

	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Settings")
		FORCEINLINE void RecalculateLayout(const FIntPoint ViewportSize)
			{ iCurrentSettings->RecalculateLayout(ViewportSize); };//TODO: call on player added

	//Synchronously loads the user settings from the specified path.
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Settings")
		void LoadSettings(FString const& Path);

	//Returns false if all settings assets are already loaded.
	bool LoadSettingsAsync(FStreamableDelegate DelegateToCall);

	//Returns the list of loaded user settings. Loads the settings from the default settings path if no settings are currently loaded.
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Settings")
		TArray<UCSUserSettings*> GetSettingsList();

	//Call after saving the user settings. Automatically called by the User Settings Widget.
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Settings")
		void SetSettings(UCSUserSettings* Settings/*, const bool bInvalidateColors = true*/);//TODO

	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Settings")
		void ChangeSettingsAsync(FName SettingsID);//TODO

	//Only use after loading settings first. (e.g. binding the streamable delegate using LoadSettingsAsync)
	inline TArray<UCSUserSettings*> uGetSettingsList()//TODO
	{
		TArray<FAssetData> aDataList;
		iSettingsLibrary->GetAssetDataList(aDataList);

		TArray<UCSUserSettings*> settingsList;
		settingsList.Reserve(aDataList.Num());

		for (FAssetData aData : aDataList)
		{
			UCSUserSettings* settings = Cast<UCSUserSettings>(aData.GetAsset());

			if (GetGameInstance() && GetGameInstance()->GetGameViewportClient() && GetGameInstance()->GetGameViewportClient()->Viewport)
				iCurrentSettings->RecalculateLayout(GetGameInstance()->GetGameViewportClient()->Viewport->GetSizeXY());

			settingsList.Add(settings);
		}
		return settingsList;
	}

private:
	inline void iRecalculateLayout(UGameViewportClient* viewportClient)
	{
		if (!viewportClient)
			return;

		FVector2D viewportSize;
		viewportClient->GetViewportSize(viewportSize);
		const FVector2D scaledSize = viewportSize / viewportClient->GetDPIScale();
		iCurrentSettings->RecalculateLayout(scaledSize.IntPoint());
	};

	//Only use after loading settings first. (e.g. binding the streamable delegate)
	inline void uSetSettingsByID(FName settingsID)
	{
		for (UCSUserSettings* settings : uGetSettingsList())
			if (settings->ID == settingsID)
				uSetSettings(settings);
	};

	inline void uSetSettings(UCSUserSettings* settings)
	{
		if (GetGameInstance()->GetGameViewportClient() && GetGameInstance()->GetGameViewportClient()->Viewport)//TODO: test
			iCurrentSettings->RecalculateLayout(GetGameInstance()->GetGameViewportClient()->Viewport->GetSizeXY());

		iCurrentSettings = settings;
		uReconstructSubtitles();
		iSettingsLibrary = nullptr;
	};

	TSet<FName> iShownSpeakers = TSet<FName>();

	UPROPERTY()
		UObjectLibrary* iSettingsLibrary = nullptr;

	UPROPERTY()
		UCSUserSettings* iCurrentSettings = UCSProjectSettingFunctions::GetDefaultSettings();
	
#pragma endregion

//TODO
#pragma region SPLITSCREEN SUPPORT
private:
	inline void uPlayerAdded(ULocalPlayer* player)
	{
		iRecalculateLayout(player->ViewportClient);

		iSourcesManager.AddPlayer(player);
	}

	inline void iPlayerRemoved(ULocalPlayer* player)
	{
		iSourcesManager.RemovePlayer(player);
	}

#pragma endregion

#pragma region QOL FUNCTIONS 
private:
	typedef TMemFunPtrType<false, UCSS_SubtitleGISS, void()>::Type CSSvFunction;
	typedef TMemFunPtrType<false, UCSS_SubtitleGISS, void(int32)>::Type CSSIDFunction;

	//Sets a timer with the timer-manager and binds it to a function.
	inline void iSetTimer(CSSvFunction func, const float dt)
	{
		if (!iTimerManager)
			return;

		FTimerHandle dropped;
		FTimerDelegate del = FTimerDelegate::CreateUObject(this, func);
		iTimerManager->SetTimer(dropped, del, dt, false);
	};

	//Sets a timer with the timer-manager and binds it to a function.
	inline void iSetTimer(CSSIDFunction func, FTimerHandle& handle, const int32 id, const float dt)
	{
		if (!iTimerManager)
			return;

		FTimerDelegate del = FTimerDelegate::CreateUObject<UCSS_SubtitleGISS, int32>(this, func, id);
		iTimerManager->SetTimer(handle, del, dt, false);
	};

	inline float itNow() const
	{ return GetWorld()->GetRealTimeSeconds(); };

#pragma endregion
};