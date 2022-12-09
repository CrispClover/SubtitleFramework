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
	FVector SoundData;

	CSTrackedSoundData()
		: FCSIndicatorWidgetData()
		, SoundData()
	{};

	CSTrackedSoundData(FVector data)
		: FCSIndicatorWidgetData()
		, SoundData(data)
	{};
};

struct CSTrackingData
{
	inline void Clear()
	{
		iSoundIDs.Empty();
		iDeletionScheduledIDs.Empty();
		iSoundData.Empty();
		icActiveSounds = 0;
	};

	inline void Empty()
	{
		//We keep all active indicators' data alive.
		const int32 inactive = iSoundIDs.Num() - icActiveSounds;
		iSoundIDs.RemoveAt(icActiveSounds, inactive, true);
		iSoundData.RemoveAt(icActiveSounds, inactive, true);
	};

	inline void Shrink()
	{
		iSoundIDs.Shrink();
		iSoundData.Shrink();
	};

	inline bool NeedsCalc(const int32 index)
		{ return index < icActiveSounds;	}

	inline int32 Num() const
		{ return iSoundIDs.Num(); }

	inline bool Contains(FCSSoundID const& id) const
		{ return iSoundIDs.Contains(id); }

	inline CSTrackedSoundData& AccessItem(const int32 index)
		{ return iSoundData[index]; }

	inline CSTrackedSoundData const& GetItem(const int32 index) const
		{ return iSoundData[index]; }

	inline FCSSoundID const& GetID(const int32 index) const
		{ return iSoundIDs[index]; }

	inline CSTrackedSoundData const* rFind(FCSSoundID const& id) const
	{
		const int32 i = iSoundIDs.Find(id);
		if (i > 0)
			return &iSoundData[i];
		else
			return nullptr;
	}

	//Returns true when the sound is newly tracked.
	inline bool TrackSound(FCSSoundID const& id, FVector const& data)
	{
		const int32 index = iSoundIDs.AddUnique(id);
		const bool isOld = index < iSoundData.Num();

		if (isOld)
			iSoundData[index].SoundData = data;
		else
			iSoundData.Add(CSTrackedSoundData(data));

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
				uRemove(i);
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
				iDeletionScheduledIDs.AddUnique(id);
		}

		for (; i < iSoundIDs.Num(); i++)
		{
			//Remove inactive data.
			if (source == iSoundIDs[i].Source)
				uRemove(i);
		}
	};

	inline bool Register(CSRegisterArgs const& args)
	{
		const int32 uxToSwap = iSoundIDs.Find(args.ID);

		if (uxToSwap < icActiveSounds)
			return false;//catches INDEX_NONE and registering multiple indicators with the same sound ID 

		if (uxToSwap != icActiveSounds)
		{
			iSoundIDs.SwapMemory(uxToSwap, icActiveSounds);
			iSoundData.SwapMemory(uxToSwap, icActiveSounds);
		}

		args.WidgetDataPtrRef = &iSoundData[icActiveSounds];

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
			args = uSwapNotify(ux);
		
		if (iDeletionScheduledIDs.Remove(id))
			uRemove(icActiveSounds);
	};

private:
	inline void uRemove(const int32 index)
	{
		iSoundIDs.RemoveAtSwap(index, 1, false);
		iSoundData.RemoveAtSwap(index, 1, false);
	}

	inline void uSwap(const int32 index)
	{
		iSoundIDs.SwapMemory(index, icActiveSounds);
		iSoundData.SwapMemory(index, icActiveSounds);
	}

	inline CSSwapArgs uSwapNotify(const int32 index)
	{
		uSwap(index);
		return CSSwapArgs(iSoundIDs[index], &iSoundData[index]);
	}

	TArray<FCSSoundID> iSoundIDs = TArray<FCSSoundID>();
	TArray<FCSSoundID> iDeletionScheduledIDs = TArray<FCSSoundID>();

	TArray<CSTrackedSoundData> iSoundData = TArray<CSTrackedSoundData>();
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
		{ return nullptr != iData.rFind(soundID); };
	
	CSTrackingManager() = delete;

	CSTrackingManager(ULocalPlayer const* player)
		: Player(player)
	{};

	virtual ~CSTrackingManager()
		{ Clear(); };
	
	inline void Clear()
	{
		UpdateIDataEvent.Clear();
		iSwapDataEvent.Clear();
		iData.Clear();
		iPendingIndicatorDelegates.Empty();
		iNewSoundTrackedEvent.Clear();
	};

	inline void Empty()
		{ iData.Empty(); };

	//We can expect memory usage to plateau, so we only shrink upon request.
	inline void Shrink()
		{ iData.Shrink(); };

	void TrackSound(FCSSoundID const& id, FVector const& data);

	bool GetSoundData(FCSSoundID const& id, FVector& data) const;

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
			iSwapDataEvent.AddUObject(args.User, args.Functions.SwapFunction);
		}
		else
		{
			FDelegateHandle handle = iNewSoundTrackedEvent.AddUObject(args.User, args.Functions.TrackFunction);
			iPendingIndicatorDelegates.Add(args.RegisterArgs.ID, handle);
		}
	}

	void UnregisterIndicator(FCSSoundID const& id, UObject* widget);
	
	void Copy(CSTrackingManager const* manager);

private:
	CSUpdateIData UpdateIDataEvent;
	CSSwapIData iSwapDataEvent;

	CSNewSoundTracked iNewSoundTrackedEvent;
	void iNotifyOfNewTrackedSound(FCSSoundID const& id);

	CSTrackingData iData = CSTrackingData();
	TMap<FCSSoundID, FDelegateHandle> iPendingIndicatorDelegates;
};