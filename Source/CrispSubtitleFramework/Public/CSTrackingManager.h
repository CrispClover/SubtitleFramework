// Copyright Crisp Clover.

#pragma once

#include "CoreMinimal.h"
#include "CSCoreLibrary.h"
#include "CSTrackingManager.generated.h"

struct FCSSoundID;

struct CSSwapArgs
{
	FCSSoundID ID;
	FCSIndicatorWidgetData* WidgetDataPtr;

	CSSwapArgs() = delete;

	CSSwapArgs(FCSSoundID const& id, FCSIndicatorWidgetData* dataPtr)
		: ID(id)
		, WidgetDataPtr(dataPtr)
	{};
};

struct CSRegisterArgs
{
	FCSSoundID const& ID;
	FCSIndicatorWidgetData*& WidgetDataPtrRef;

	CSRegisterArgs() = delete;

	CSRegisterArgs(FCSSoundID const& id, FCSIndicatorWidgetData*& dataPtrRef)
		: ID(id)
		, WidgetDataPtrRef(dataPtrRef)
	{};
};

template<typename UserClass>
struct CSIndicatorFunctions
{
	typedef typename TMemFunPtrType<false, UserClass, void()>::Type UpdateFunc;
	typedef typename TMemFunPtrType<false, UserClass, void(CSSwapArgs const&)>::Type SwapFunc;
	typedef typename TMemFunPtrType<false, UserClass, void(FCSSoundID const&)>::Type TrackFunc;

	UpdateFunc UpdateFunction;
	SwapFunc SwapFunction;
	TrackFunc TrackFunction;

	CSIndicatorFunctions() = delete;

	CSIndicatorFunctions(UpdateFunc updateFunction, SwapFunc swapFunction, TrackFunc trackFunction)
		: UpdateFunction(updateFunction)
		, SwapFunction(swapFunction)
		, TrackFunction(trackFunction)
	{};
};

template<typename UserClass>
struct CSIndicatorRegistrationData
{
	UserClass* User;
	CSRegisterArgs RegisterArgs;
	CSIndicatorFunctions<UserClass> Functions;

	CSIndicatorRegistrationData() = delete;

	CSIndicatorRegistrationData(UserClass* user, CSRegisterArgs const& registerArgs, CSIndicatorFunctions<UserClass> const& functions)
		: User(user)
		, RegisterArgs(registerArgs)
		, Functions(functions)
	{};
};

//The UI data required to calculate and display a direction indicator correctly.
USTRUCT(BlueprintType)
struct FCSIndicatorWidgetData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CrispSubtitles|UI")
		FVector2D Offset;
	 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CrispSubtitles|UI")
		float Angle;

	//If the object is behind the camera, this value will be negative. Magnitude is the screen-distance between widget and object.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CrispSubtitles|UI")
		float OpacityDriver;
	
	FCSIndicatorWidgetData()
		: Offset(FVector2D())
		, Angle(0)
		, OpacityDriver(1)
	{};
};

//All data required to calculate a 3D direction indicator correctly.
struct CSTrackedSoundData : public FCSIndicatorWidgetData
{
	FVector pSound;

	CSTrackedSoundData()
		: FCSIndicatorWidgetData()
		, pSound()
	{};

	CSTrackedSoundData(FVector pSound)
		: FCSIndicatorWidgetData()
		, pSound(pSound)
	{};
};

struct CSTrackingData
{
	inline void Clear()
	{
		iSoundIDs.Empty();
		iDeletionScheduledIDs.Empty();
		iTrackedSounds.Empty();
		icActiveSounds = 0;
	};

	inline void Empty()
	{
		//We keep all active indicators' data alive.
		const int32 inactive = iSoundIDs.Num() - icActiveSounds;
		iSoundIDs.RemoveAt(icActiveSounds, inactive, true);
		iTrackedSounds.RemoveAt(icActiveSounds, inactive, true);
	};

	inline void Shrink()
	{
		iSoundIDs.Shrink();
		iTrackedSounds.Shrink();
	};

	inline bool NeedsCalc(const int32 index)
		{ return index < icActiveSounds;	}

	inline int32 Num() const
		{ return iSoundIDs.Num(); }

	inline bool Contains(FCSSoundID const& id) const
		{ return iSoundIDs.Contains(id); }

	inline CSTrackedSoundData& vAccessItem(const int32 index)
		{ return iTrackedSounds[index]; }

	inline CSTrackedSoundData const& vGetItem(const int32 index) const
		{ return iTrackedSounds[index]; }

	inline FCSSoundID const& vGetID(const int32 index) const
		{ return iSoundIDs[index]; }

	inline CSTrackedSoundData const* uFind(FCSSoundID const& id) const
	{
		const int32 i = iSoundIDs.Find(id);
		if (i > 0)
			return &iTrackedSounds[i];
		else
			return nullptr;
	}

	//Returns true when the sound is newly tracked.
	inline bool TrackSound(FCSSoundID const& id, FVector const& pSound)
	{
		const int32 index = iSoundIDs.AddUnique(id);
		const bool isOld = index < iTrackedSounds.Num();

		if (isOld)
		{
			iTrackedSounds[index].pSound = pSound;
		}
		else
		{
			iTrackedSounds.Add(CSTrackedSoundData(pSound));
		}

		return !isOld;
	};

	inline void RemoveSound(FCSSoundID const& id)
	{
		int32 i = 0;
		for (; i < icActiveSounds; i++)
		{
			//Schedule deletion of active indicators' data.
			if (iSoundIDs[i] == id)
			{
				iDeletionScheduledIDs.AddUnique(id);
				return;
			}
		}

		for (; i < iSoundIDs.Num(); i++)
		{
			//Remove inactive data.
			if (iSoundIDs[i] == id)
			{
				vRemove(i);
				return;
			}
		}
	};

	inline void RemoveSource(const FName source)
	{
		int32 i = 0;
		for (; i < icActiveSounds; i++)
		{
			//Schedule deletion of active indicators' data.
			FCSSoundID const& id = iSoundIDs[i];

			if (source == id.Source)
			{
				iDeletionScheduledIDs.AddUnique(id);
			}
		}

		for (; i < iSoundIDs.Num(); i++)
		{
			//Remove inactive data.
			if (source == iSoundIDs[i].Source)
			{
				vRemove(i);
			}
		}
	};

	inline bool Register(CSRegisterArgs const& args)
	{
		const int32 uxToSwap = iSoundIDs.Find(args.ID);

		if (uxToSwap < icActiveSounds)
			return false;//catches INDEX_NONE and registering multiple indicators with the same sound ID 

		if (uxToSwap != icActiveSounds)
		{
			vSwap(uxToSwap);
		}

		args.WidgetDataPtrRef = &iTrackedSounds[icActiveSounds];

		icActiveSounds++;

		return true;
	};

	inline void Unregister(FCSSoundID const& id, CSSwapArgs& args)
	{
		bool wasSwapped = false;

		const int32 ux = iSoundIDs.Find(id);//index of indicator to remove

		if (ux < 0 || ux >= icActiveSounds)
			return;

		icActiveSounds--;//index of last indicator (for now)

		if (ux != icActiveSounds)
		{
			args = vSwapNotify(ux);
		}
		
		if (iDeletionScheduledIDs.Remove(id))
		{
			vRemove(icActiveSounds);
		}
	};

	void DumpSoundData(TArray<FCSSoundID>& ids, TArray<FVector>& pSounds) const
	{
		ids.Append(iSoundIDs);

		pSounds.Reserve(pSounds.Num() + iTrackedSounds.Num());

		for (CSTrackedSoundData const& data : iTrackedSounds)
			pSounds.Add(data.pSound);
	}

private:
	inline void vRemove(const int32 index)
	{
		iSoundIDs.RemoveAtSwap(index, 1, false);
		iTrackedSounds.RemoveAtSwap(index, 1, false);
	}

	inline void vSwap(const int32 index)
	{
		iSoundIDs.SwapMemory(index, icActiveSounds);
		iTrackedSounds.SwapMemory(index, icActiveSounds);
	}

	inline CSSwapArgs vSwapNotify(const int32 index)
	{
		vSwap(index);
		return CSSwapArgs(iSoundIDs[index], &iTrackedSounds[index]);
	}

	TArray<FCSSoundID> iSoundIDs = TArray<FCSSoundID>();
	TArray<FCSSoundID> iDeletionScheduledIDs = TArray<FCSSoundID>();

	TArray<CSTrackedSoundData> iTrackedSounds = TArray<CSTrackedSoundData>();
	int32 icActiveSounds = 0;
};

//Delegate to update direction indicators after their data has been calculated.
DECLARE_MULTICAST_DELEGATE(CSUpdateIData);

//Delegate to notify indicators of pointer changes.
DECLARE_MULTICAST_DELEGATE_OneParam(CSSwapIData, CSSwapArgs const&);

//Delegate to notify indicators when a new sound is being tracked.
DECLARE_MULTICAST_DELEGATE_OneParam(CSNewSoundTracked, FCSSoundID const&);

/**
 * 
 */
class CRISPSUBTITLEFRAMEWORK_API CSTrackingManager
{
public:
	ULocalPlayer const* Player;
	
	void Calculate();

	inline bool Contains(FCSSoundID const& soundID) const
		{ return nullptr != iData.uFind(soundID); };
	
	CSTrackingManager() = delete;

	CSTrackingManager(ULocalPlayer const* player)
		: Player(player)
	{};

	virtual ~CSTrackingManager()
		{ Clear(); };
	
	inline void Clear()
	{
		UpdateIDataEvent.Clear();
		SwapDataEvent.Clear();
		iData.Clear();
		iPendingIndicatorDelegates.Empty();
		NewSoundTrackedEvent.Clear();
	};

	inline void Empty()
		{ iData.Empty(); };

	//We can expect memory usage to plateau, so we only shrink upon request.
	inline void Shrink()
		{ iData.Shrink(); };

	void TrackSound(FCSSoundID const& id, FVector const& pSound);

	bool GetSoundData(FCSSoundID const& id, FVector& pSound) const;

	inline void GetSoundDataDump(TArray<FCSSoundID>& soundIDs, TArray<FVector>& pSounds) const
		{ iData.DumpSoundData(soundIDs, pSounds); };

	inline void RemoveSound(FCSSoundID const& id)
		{ return iData.RemoveSound(id); };

	inline void RemoveSource(FName const& source)
		{ return iData.RemoveSource(source); };
	
	template<typename UserClass>
	void RegisterIndicator(CSIndicatorRegistrationData<UserClass> args)
	{
		if (iData.Register(args.RegisterArgs))
		{
			UpdateIDataEvent.AddUObject(args.User, args.Functions.UpdateFunction);
			SwapDataEvent.AddUObject(args.User, args.Functions.SwapFunction);
		}
		else
		{
			FDelegateHandle handle = NewSoundTrackedEvent.AddUObject(args.User, args.Functions.TrackFunction);
			iPendingIndicatorDelegates.Add(args.RegisterArgs.ID, handle);
		}
	}

	void UnregisterIndicator(FCSSoundID const& id, UObject* widget);
	
	void vCopy(CSTrackingManager const* manager);

private:
	CSUpdateIData UpdateIDataEvent;
	CSSwapIData SwapDataEvent;
	CSNewSoundTracked NewSoundTrackedEvent;

	void iNotifyOfNewTrackedSound(FCSSoundID const& id);

	CSTrackingData iData = CSTrackingData();
	TMap<FCSSoundID, FDelegateHandle> iPendingIndicatorDelegates;
};