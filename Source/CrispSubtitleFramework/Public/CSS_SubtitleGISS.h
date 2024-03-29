// Copyright Crisp Clover.

#pragma once

#include "Subsystems/GameInstanceSubsystem.h"
#include "CSProjectSettingFunctions.h"
#include "CSCustomDataManager.h"
#include "CSSourcesManager.h"
#include "CSS_SubtitleGISS.generated.h"

class UObjectLibrary;
class UCSUserSettings;

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
	inline TArray<DataElement> const& Get() const
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
				vRemoveAt(i);
				return element;
			}
		}

		return DataElement();
	};

	DataElement* uAccess(const int32 id)
	{
		for (int32 i = 0; i < iIDs.Num(); i++)
			if (iIDs[i] == id)
				return &iDataElements[i];

		return nullptr;
	};

	FTimerHandle Remove(int32 id)
	{
		for (int32 i = iIDs.Num() - 1; i >= 0; i--)
		{
			if (iIDs[i] == id)
			{
				FTimerHandle handle = iTimerHandles[i];
				vRemoveAt(i);
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

	inline void Reserve(const int32 c)
	{
		iDataElements.Reserve(c);
		iTimerHandles.Reserve(c);
		iIDs.Reserve(c);
	};

	inline void Shrink()
	{
		iDataElements.Shrink();
		iTimerHandles.Shrink();
		iIDs.Shrink();
	};

private:
	inline void vRemoveAt(const int32 index)
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
			: iData3D(timedData)
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
			{ return iData3D.iDataElements.IsValidIndex(iIndex); };
		
		inline void Reset()
			{ iIndex = 0; };

		FORCEINLINE DataElement& Data() const
			{ return iData3D.iDataElements[iIndex]; };

		FORCEINLINE FTimerHandle& Handle() const
			{ return iData3D.iTimerHandles[iIndex]; };

		FORCEINLINE int32 ID() const
			{ return iData3D.iIDs[iIndex]; };
		
		void RemoveCurrent()
		{
			iData3D.vRemoveAt(iIndex);
			iIndex--;
		};

	private:
		TCSTimedData& iData3D;
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
		{ return iTimedData.Get(); };

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
				vRemoveAt(i);
				return;
			}
		}
	};

	//Removes all permanent data and copies the removed TimerHandles and IDs.
	inline void RemovePermanents(TArray<FTimerHandle>& handles, TArray<int32>& ids)
	{
		for (auto it = GetReverseIterator(); it; --it)
		{
			if (it.IsPermanent())
			{
				handles.Add(it.Handle());
				ids.Add(it.ID());
				
				vRemoveAt(it.xCurrent());
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
	inline void vRemoveAt(const int32 index)
	{
		iArePermanent.RemoveAt(index, 1);
		iTimedData.vRemoveAt(index);
	};

	TCSTimedData<DataElement> iTimedData = TCSTimedData<DataElement>();
	TArray<bool> iArePermanent = TArray<bool>();

	friend struct FCSCurrentSubtitleData;

	// --- ITERATORS --- //
public:
	struct VolatileIterator
	{
		VolatileIterator(TCSCurrentData<DataElement>& timedData, const int32 startIndex = 0)
			: iData3D(timedData)
			, iIndex(startIndex)
		{};
		
		inline VolatileIterator& operator++()
		{
			iIndex++;
			return *this;
		};
		
		inline VolatileIterator& operator--()
		{
			iIndex--;
			return *this;
		}
		
		FORCEINLINE explicit operator bool() const
			{ return iData3D.iTimedData.iDataElements.IsValidIndex(iIndex); };
		
		inline void Reset()
			{ iIndex = 0; };

		FORCEINLINE DataElement& Data() const
			{ return iData3D.iTimedData.iDataElements[iIndex]; };

		FORCEINLINE FTimerHandle& Handle() const
			{ return iData3D.iTimedData.iTimerHandles[iIndex]; };

		FORCEINLINE int32 ID() const
			{ return iData3D.iTimedData.iIDs[iIndex]; };

		FORCEINLINE int32 IsPermanent() const
			{ return iData3D.iArePermanent[iIndex]; };

		/*FORCEINLINE int32 xCurrent() const
			{ return iIndex; };*/
		
		void RemoveCurrent()
		{
			iData3D.vRemoveAt(iIndex);
			iIndex--;
		};

	private:
		TCSCurrentData<DataElement>& iData3D;
		int32 iIndex = 0;
	};

	struct Iterator
	{
		Iterator(TCSCurrentData<DataElement> const& timedData, const int32 startIndex = 0)
			: iData3D(timedData)
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
			{ return iData3D.iTimedData.iDataElements.IsValidIndex(iIndex); };
		
		inline void Reset()
			{ iIndex = 0; };

		FORCEINLINE DataElement const& Data() const
			{ return iData3D.iTimedData.iDataElements[iIndex]; };

		FORCEINLINE FTimerHandle const& Handle() const
			{ return iData3D.iTimedData.iTimerHandles[iIndex]; };

		FORCEINLINE int32 ID() const
			{ return iData3D.iTimedData.iIDs[iIndex]; };

		FORCEINLINE int32 IsPermanent() const
			{ return iData3D.iArePermanent[iIndex]; };

		FORCEINLINE int32 xCurrent() const
			{ return iIndex; };

	private:
		TCSCurrentData<DataElement> const& iData3D;
		int32 iIndex = 0;
	};

	inline Iterator GetIterator() const
		{ return Iterator(*this); };

	inline Iterator GetReverseIterator() const
		{ return Iterator(*this, Num() - 1); };

	inline VolatileIterator GetVolatileIterator()
		{ return VolatileIterator(*this); };
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
		if (subtitle.Lines.Num() > UCSProjectSettingFunctions::GetMaxSubtitleLines())
		{
			iCurrentData.Flush(removedHandles, removedIDs);
			return iCurrentData.Add(subtitle, id, isPermanent);
		}

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
				iCurrentData.vRemoveAt(it.xCurrent());
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
				iCurrentData.vRemoveAt(it.xCurrent());
			}
		}
	}

	//Removes all permanent subtitles and copies the removed TimerHandles and IDs.
	inline void RemovePermanents(TArray<FTimerHandle>& handles, TArray<int32>& ids)
	{
		for (auto it = iCurrentData.GetReverseIterator(); it; --it)
		{
			if (it.IsPermanent())
			{
				icLines -= it.Data().Lines.Num();
				handles.Add(it.Handle());
				ids.Add(it.ID());
				
				iCurrentData.vRemoveAt(it.xCurrent());
			}
		}
	};

	//Copies the TimerHandle and ID at index 0, then removes all data at index 0.
	inline void Kick(FTimerHandle& handle, int32& id)
	{
		if (!Num())
			return;

		icLines -= iCurrentData.Get()[0].Lines.Num();
		handle = iCurrentData.Handles()[0];
		id = iCurrentData.iTimedData.IDs()[0];

		iCurrentData.vRemoveAt(0);
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

//Bundle to control broadcasts.
struct FCSBroadcastingData
{
	FCSBroadcastingData()
		: itLastBroadcast(0)
		, idtBusy(0)
		, iIsPaused(false)
	{};

	inline void LogBroadcast(const float tNow, const float dtBusy)
	{
		itLastBroadcast = tNow;
		idtBusy = dtBusy;
	};

	inline void LogPaused()
		{ iIsPaused = true; };

	inline void LogUnpaused()
		{ iIsPaused = false; };

	inline bool IsPaused() const
		{ return iIsPaused; };

	inline float dtBusy(const float tNow) const
		{ return FMath::Max(0.f, idtBusy + itLastBroadcast - tNow); };

private:
	float itLastBroadcast;
	float idtBusy;
	bool iIsPaused;
};
#pragma endregion

#pragma region DELEGATES

//Delegate for the subtitle trigger.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSubtitleTrigger, FCrispSubtitle const&, Subtitle);

//Delegate for the subtitle reconstruct trigger.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FReconstructSubtitlesTrigger, TArray<FCrispSubtitle> const&, Subtitles, UCSUserSettings*, Settings);

//Delegate for the caption trigger.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCaptionTrigger, FCrispCaption const&, Caption);

//Delegate for the caption reconstruct trigger.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FReconstructCaptionsTrigger, TArray<FCrispCaption> const&, Captions, UCSUserSettings*, Settings);

//Delegate for triggering destruction.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDestructTrigger, const int32, Index);

//Delegate to notify about permanent subtitle changes.
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPermanentSubtitleNotify);

//Delegate to notify about location dump changes.
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FLocationDumpNotify);

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
	inline void iPlayerAdded(ULocalPlayer* player)
		{ iSourcesManager.AddPlayer(player); };

	inline void iPlayerRemoved(ULocalPlayer* player)
		{ iSourcesManager.RemovePlayer(player); };

	inline void iManageRemoval(const int32 id)
	{
		iIDManager.Delete(id);

		if (CustomDataManager)
			CustomDataManager->RemoveData(id);
	}

	inline void iManageRemoval(FTimerHandle& handle, const int32 id)
	{
		iIDManager.Delete(id);

		if (CustomDataManager)
			CustomDataManager->RemoveData(id);

		if (uTimerManager)
			uTimerManager->ClearTimer(handle);
	}

	inline void iManageRemoval(TArray<FTimerHandle>& handles, TArray<int32> const& ids)
	{
		for (const int32 id : ids)
		{
			iIDManager.Delete(id);

			if (CustomDataManager)
				CustomDataManager->RemoveData(id);
		}

		if (uTimerManager)
			for (FTimerHandle& handle : handles)
				uTimerManager->ClearTimer(handle);
	}

	FTimerManager* uTimerManager = nullptr;
	FCSIDManager iIDManager = FCSIDManager();

#pragma region SUBTITLES
public:
	//Called when a subtitle should begin to be displayed.
	UPROPERTY(BlueprintAssignable, Category = "CrispSubtitles|Events")
		FSubtitleTrigger ConstructSubtitleEvent;
	
	//Called when a subtitle should stop being displayed.
	UPROPERTY(BlueprintAssignable, Category = "CrispSubtitles|Events")
		FDestructTrigger DestructSubtitleEvent;
	
	//Called when a subtitle with infinite duration starts being displayed.
	UPROPERTY(BlueprintAssignable, Category = "CrispSubtitles|Events")
		FPermanentSubtitleNotify PermanentSubtitleAdded;

	//Called when all subtitles with infinite duration were removed.
	UPROPERTY(BlueprintAssignable, Category = "CrispSubtitles|Events")
		FPermanentSubtitleNotify PermanentSubtitlesRemoved;
	
	/**
	 * Called when the subtitle UI needs to be reconstructed.
	 * (e.g. when the settings have changed)
	 */
	UPROPERTY(BlueprintAssignable, Category = "CrispSubtitles|Events")
		FReconstructSubtitlesTrigger ReconstructSubtitlesEvent;

	//Returns the data for the subtitles currently displayed in the UI.
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Subtitles")
		FORCEINLINE TArray<FFullSubtitle> const& GetCurrentSubtitles() const
			{ return iCurrentSubtitles.Get(); };

	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Subtitles")
		FORCEINLINE TArray<FFullSubtitle> const& GetQueuedSubtitles() const
			{ return iQueuedSubtitles.Get(); };

	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Subtitles")
		TArray<FFullSubtitle> GetUpcomingSubtitles(float DurationToSearch) const;
	
	//Returns whether a subtitle with infinite duration exists.
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Subtitles")
		bool HasPermanentSubtitle();

	/**
	 * This function allows you to change the source of a subtitle after it has already been queued.
	 * Use-case example: The player walks away from a talking character. Beyond a certain distance the character might be heard over a radio.
	 * @param IDs The IDs assigned to the subtitles.
	 * @param NewSourceName The new source.
	 * @param NewDescription the new description. (optional)
	 * @param bShouldChangeDescription Whether the description should be updated.
	 */
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Subtitles")
		void ModifyQueuedSubtitles(TArray<int32> const& IDs, const FName NewSourceName, FText const& NewDescription, const bool bShouldChangeDescription = false);

	//Removes the oldest subtitle from the UI.
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Subtitles")
		void KickSubtitle();

	//Removes the oldest permanent subtitle from the UI.
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Subtitles")
		void RemovePermanents();

	//Removes all current subtitles from the UI.
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Subtitles")
		void ClearSubtitles();
	
	//Pauses all timers for all subtitles controlled by the subsystem.
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Subtitles")
		void PauseSubtitles();
	
	//Pauses all timers for all queued subtitles.
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Subtitles")
		void PauseQueuedSubtitles();
	
	//Resumes all timers for all subtitles controlled by the subsystem.
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Subtitles")
		void UnpauseSubtitles();
	
	//Resumes all timers for all queued subtitles.
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Subtitles")
		void UnpauseQueuedSubtitles();

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
	 * @param SpeakerText The speaker.
	 * @param Speaker The speaker's ID.
	 * @param SourceName The ID of the source used for registration.
	 * @return The IDs of the subtitles.
	 */
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Queue")
		TArray<int32> QueueRawSubtitles(TArray<FRawSubtitle> const& Subtitles, FText const& SpeakerText, const FName Speaker, const FName SourceName);
	
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
	void iBroadcastSubtitle(FFullSubtitle const& subtitle, const float tNow, const int32 id);
	void iDestroySubtitle(const int32 id);
	void iReconstructSubtitles() const;

	inline void iPauseOnPermanentSubtitle()
	{ 
		if (!UCSProjectSettingFunctions::ShouldPauseOnPermanentSubtitle())
			return;

		PauseQueuedSubtitles();
		PermanentSubtitleAdded.Broadcast();
	};

	FCSCurrentSubtitleData iCurrentSubtitles = FCSCurrentSubtitleData();
	TCSTimedData<FFullSubtitle> iQueuedSubtitles = TCSTimedData<FFullSubtitle>();
	FCSBroadcastingData iSubtitleBroadcastData = FCSBroadcastingData();

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

	//Returns the data for the captions currently displayed in the UI.
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Captions")
		FORCEINLINE TArray<FFullCaption> const& GetCurrentCaptions() const
			{ return iCurrentCaptions.Get(); };

	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Captions")
		FORCEINLINE TArray<FFullCaption> const& GetQueuedCaptions() const
			{ return iQueuedCaptions.Get(); };
	
	//Returns whether a caption with infinite duration exists.
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Captions")
		bool HasPermanentCaption();

	//Removes all current subtitles from the UI.
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Captions")
		void ClearCaptions();

	//Pauses all timers for all captions controlled by the subsystem.
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Captions")
		void PauseCaptions();
	
	//Pauses all timers for all queued captions.
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Captions")
		void PauseQueuedCaptions();
	
	//Resumes all timers for all captions controlled by the subsystem.
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Captions")
		void UnpauseCaptions();
	
	//Resumes all timers for all queued captions.
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Captions")
		void UnpauseQueuedCaptions();
	
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
	void iBroadcastCaption(FFullCaption const& caption, const float tNow, const int32 id);
	void iDestroyCaption(const int32 id);
	void iReconstructCaptions() const;

	bool iTryUpdateCaptionDuration(FCSSoundID const& soundID, const float dtDisplay, const bool isPermanent);

	TCSCurrentData<FFullCaption> iCurrentCaptions = TCSCurrentData<FFullCaption>();
	TCSTimedData<FFullCaption> iQueuedCaptions = TCSTimedData<FFullCaption>();
	bool iCaptionsArePaused = false;

#pragma endregion

#pragma region DATA ACCESS
public:
	//Returns the approximated time the user will remain busy with reading the currently displayed subtitles.
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Data")
		float GetBusyDuration() const;

	//The object that will manage the custom data for subtitles.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CrispSubtitles|CustomData")
		UCSCustomDataManager* CustomDataManager = nullptr;

#pragma endregion

#pragma region SOURCE MANAGEMENT
public:
	//Called when a location was updated.
	UPROPERTY(BlueprintAssignable, Category = "CrispSubtitles|Events")
		FLocationDumpNotify LocationUpdated;

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
		bool RegisterAndTrackSound(FCSSoundID const& SoundID, FVector const& SourceLocation, ULocalPlayer const* Player = nullptr);

	/**
	 * Starts tracking a registered sound source for direction indicators.
	 * @param SoundID The ID used at registration.
	 * @param SourceLocation The world position to use as the source's location.
	 * @param Player The player to change the sound location for. If left blank, the location will update for all players.
	 * @return false if source hasn't been registered yet.
	 */
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Sources")
		bool TrackSound(FCSSoundID const& SoundID, FVector const& SourceLocation, ULocalPlayer const* Player = nullptr);

	/**
	 * @param SoundID The ID used at registration.
	 * @param SourceLocation The currently registered position of the source.
	 * @param Player The player to get the tracked sound position for. Can be left blank if splitscreen is not supported.
	 * @return false if the source isn't tracked/registered.
	 */
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Sources")
		bool GetSoundLocation(FCSSoundID const& SoundID, FVector& Location, ULocalPlayer const* Player = nullptr) const;

	/**
	 * @param SoundIDs The IDs used at registration, indeces match with the location array.
	 * @param Player The player to get the tracked sound position for. Can be left blank to get all positions.
	 * @return The currently registered positions of the sources.
	 */
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Sources")
		TArray<FVector> GetSoundLocationDump(TArray<FCSSoundID>& SoundIDs, ULocalPlayer const* Player = nullptr) const;

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
	 * @param RemoveCurrentSubtitles Whether to remove current subtitles from the UI.
	 * @param RemoveCurrentCaptions Whether to remove current captions from the UI.
	 * @return true if the source has been removed.
	 */
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Sources")
		bool UnregisterSource(const FName SourceName, const bool RemoveCurrentSubtitles = false, const bool RemoveCurrentCaptions = false);
	
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Sources")
		void EmptySources();

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
	 * Overrides which registered sources are allowed to broadcast subtitles and captions.
	 * @param SourcesOverride The sources that are allowed to broadcast. Sources must still be registered to broadcast.
	 */
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Sources")
		void SetSourcesOverride(TSet<FName> const& SourcesOverride);

	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Sources")
		FORCEINLINE TSet<FName> const& GetSourcesOverride() const
			{ return iSourcesManager.GetOverride(); };

	//Clears the SourcesOverride, allowing all registered sources to broadcast subtitles and captions.
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Sources")
		void ClearSourcesOverride();

private:
	void iRemoveCurrentSubtitlesBySource(const FName source);
	void iRemoveCurrentCaptionsBySource(const FName source);

	FCSSourcesManager iSourcesManager = FCSSourcesManager();

#pragma endregion

#pragma region INDICATORS
public:
	/**
	 * Forces recalculation of the indicator data. Doesn't need to be called by default.
	 * If you want to run calculations on a timer, use this and disable bCalculateIndicatorsOnTick in the project settings.
	 */
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Indicators")
		void UpdateIndicatorData(ULocalPlayer const* Player);

	template<typename UserClass>
	void RegisterIndicator(CSIndicatorRegistrationData<UserClass> args, ULocalPlayer const* player = nullptr)
		{ iSourcesManager.RegisterIndicator(args, player); }
	
	/**
	 * Removes an indicator from the list of registered indicators.
	 * @param SoundID The ID of the sound.
	 * @param Player The player the indicator widget belongs to.
	 * @param Widget The widget to unregister.
	 */
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Indicators")
	void UnregisterIndicator(FCSSoundID const& SoundID, ULocalPlayer const* Player, UObject* Widget);

#pragma endregion

#pragma region SETTINGS
public:
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Settings")
		FORCEINLINE UCSUserSettings* GetCurrentSettings() const
			{ return iCurrentSettings; };

	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Settings")
		void RecalculateLayout();

	//Call after saving the user settings. Automatically called by UserSettingsWidget.
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Settings")
		void SetSettings(UCSUserSettings* Settings);

private:
	UPROPERTY()
		UCSUserSettings* iCurrentSettings = UCSProjectSettingFunctions::GetDefaultSettings();
	
#pragma endregion

#pragma region QOL FUNCTIONS 
private:
	typedef TMemFunPtrType<false, UCSS_SubtitleGISS, void()>::Type CSSvFunction;
	typedef TMemFunPtrType<false, UCSS_SubtitleGISS, void(int32)>::Type CSSIDFunction;

	//Sets a timer with the timer-manager and binds it to a function.
	inline void iSetTimer(CSSvFunction func, const float dt)
	{
		if (!uTimerManager)
			return;

		FTimerHandle dropped;
		FTimerDelegate del = FTimerDelegate::CreateUObject(this, func);
		uTimerManager->SetTimer(dropped, del, dt, false);
	};

	//Sets a timer with the timer-manager and binds it to a function.
	inline void iSetTimer(CSSIDFunction func, FTimerHandle& handle, const int32 id, const float dt)
	{
		if (!uTimerManager)
			return;

		FTimerDelegate del = FTimerDelegate::CreateUObject<UCSS_SubtitleGISS, int32>(this, func, id);
		uTimerManager->SetTimer(handle, del, dt, false);
	};

	inline float tNow() const
	{ return GetWorld()->GetRealTimeSeconds(); };

#pragma endregion
};