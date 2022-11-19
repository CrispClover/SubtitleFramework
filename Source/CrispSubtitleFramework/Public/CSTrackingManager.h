// Copyright Crisp Clover.

#pragma once

#include "CoreMinimal.h"
#include "CSTrackingManager.generated.h"

struct FCSSoundID;

struct FCSRegisterArgs
{
	FCSSoundID const& ID;
	FCSIndicatorWidgetData*& WidgetDataPtrRef;

	FCSRegisterArgs() = delete;

	FCSRegisterArgs(FCSSoundID const& id, FCSIndicatorWidgetData*& dataPtrRef)
		: ID(id)
		, WidgetDataPtrRef(dataPtrRef)
	{};
};

struct FCSSwapArgs
{
	FCSSoundID ID;
	FCSIndicatorWidgetData* WidgetDataPtr;

	FCSSwapArgs() = delete;/*
		: ID()
		, WidgetDataPtr(nullptr)
	{};*/

	FCSSwapArgs(FCSSoundID const& id, FCSIndicatorWidgetData* dataPtr)//TODO
		: ID(id)
		, WidgetDataPtr(dataPtr)
	{};
};

//The 2D data required to calculate and display a direction indicator correctly.
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

//The 3D data required to calculate a direction indicator correctly.
struct FCSSoundIndicatorData : public FCSIndicatorWidgetData
{
	FVector SoundData;

	FCSSoundIndicatorData()
		: FCSIndicatorWidgetData()
		, SoundData()
	{};

	FCSSoundIndicatorData(FVector data)
		: FCSIndicatorWidgetData()
		, SoundData(data)
	{};
};

struct FCSTrackingData
{
	inline void Clear()
	{
		iSoundIDs.Empty();
		iDeletionScheduledIDs.Empty();
		iSoundData.Empty();
		inIndicators = 0;
	};

	inline void Empty()
	{
		//We keep all active indicators' data alive.
		const int32 inactive = iSoundIDs.Num() - inIndicators;
		iSoundIDs.RemoveAt(inIndicators, inactive, true);
		iSoundData.RemoveAt(inIndicators, inactive, true);
	};

	inline void Shrink()
	{
		iSoundIDs.Shrink();
		iSoundData.Shrink();
	};

	inline bool NeedsCalc(const int32 index)
	{ return index < inIndicators;	}

	inline int32 Num() const
	{ return iSoundIDs.Num(); }

	inline FCSSoundIndicatorData& AccessItem(const int32 index)
	{ return iSoundData[index]; }

	inline FCSSoundIndicatorData const& GetItem(const int32 index) const
	{ return iSoundData[index]; }

	inline FCSSoundID const& GetID(const int32 index) const
	{ return iSoundIDs[index]; }

	inline FCSSoundIndicatorData const* Find(FCSSoundID const& id) const
	{
		const int32 i = iSoundIDs.Find(id);
		if (i > 0)
			return &iSoundData[i];
		else
			return nullptr;
	}

	inline void TrackSound(FCSSoundID const& id, FVector const& data)
	{
		const int32 index = iSoundIDs.AddUnique(id);

		if (index < iSoundData.Num())
			iSoundData[index].SoundData = data;
		else
			iSoundData.Add(FCSSoundIndicatorData(data));
	};

	inline void RemoveSource(const FName source)//TEST
	{
		int32 i = 0;
		for (; i < inIndicators; i++)
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
				iRemove(i);
		}
	};

	inline bool Register(FCSSoundID const& id, FCSIndicatorWidgetData*& dataPtr)
	{
		const int32 xToSwap = iSoundIDs.Find(id);

		if (xToSwap < 0)
			return false;

		if (xToSwap != inIndicators)
		{
			iSoundIDs.SwapMemory(xToSwap, inIndicators);
			iSoundData.SwapMemory(xToSwap, inIndicators);
		}

		dataPtr = &iSoundData[inIndicators];

		inIndicators++;

		return true;
	};

	inline void Unregister(FCSSoundID const& id, FCSSwapArgs& args)//TEST
	{
		bool wasSwapped = false;

		const int32 index = iSoundIDs.Find(id);//index of indicator to remove

		if (index < 0)
			return;

		inIndicators--;//index of last indicator (for now)

		if (index > inIndicators)
			return;//TODO: can be removed. probably?

		if (index != inIndicators)
			args = iSwap(index);
		
		if (iDeletionScheduledIDs.Remove(id))
			iRemove(inIndicators);
	};

private:
	inline void iRemove(const int32 index)
	{
		iSoundIDs.RemoveAtSwap(index, 1, false);
		iSoundData.RemoveAtSwap(index, 1, false);
	}

	inline FCSSwapArgs iSwap(const int32 index)
	{
		iSoundIDs.SwapMemory(index, inIndicators);
		iSoundData.SwapMemory(index, inIndicators);

		return FCSSwapArgs(iSoundIDs[index], &iSoundData[index]);
	}

	TArray<FCSSoundID> iSoundIDs = TArray<FCSSoundID>();
	TArray<FCSSoundID> iDeletionScheduledIDs = TArray<FCSSoundID>();

	TArray<FCSSoundIndicatorData> iSoundData = TArray<FCSSoundIndicatorData>();
	int32 inIndicators = 0;
};

//Delegate to update direction indicators after their data has been calculated.
DECLARE_MULTICAST_DELEGATE(FCSUpdateIData);

/**
 * Delegate to notify indicators of index changes.
 * Params: FName sourceName, int32 oldIndex, int32 newIndex, FIndicatorWidgetData* newWidgetDataPointer
 */
DECLARE_MULTICAST_DELEGATE_OneParam(FCSSwapIData, FCSSwapArgs const&);

struct CSIndicatorDelegates
{
	inline void Clear()
	{
		SwapIDataEvent.Clear();
		UpdateIDataEvent.Clear();
	};

	inline void Remove(UObject* widget)
	{
		SwapIDataEvent.RemoveAll(widget);
		UpdateIDataEvent.RemoveAll(widget);
	};

	//Used to update the indicator UIs, usually called on tick or on a timer.
	FCSUpdateIData UpdateIDataEvent;

	//Used to update the swapped indicator data pointer after removing one of the indicators.
	FCSSwapIData SwapIDataEvent;
};

/**
 * 
 */
class CRISPSUBTITLEFRAMEWORK_API CSTrackingManager
{
public:
	ULocalPlayer const* Player;//TODO
	
	void Calculate();

	inline bool Contains(FCSSoundID const& soundID) const//TODO?
	{ return nullptr != iData.Find(soundID); };
	
	CSTrackingManager() = delete;

	CSTrackingManager(ULocalPlayer const* player)
		: Player(player)
	{};

	virtual ~CSTrackingManager()
	{ Clear(); };
	
	inline void Clear()
	{
		iDelegates.Clear();
		iData.Clear();
	};

	inline void Empty()
	{ iData.Empty(); };

	//We can expect memory usage to plateau, so we only shrink upon request.
	inline void Shrink()
	{ iData.Shrink(); };

	inline void TrackSound(FCSSoundID const& id, FVector const& data)
	{ iData.TrackSound(id, data); };

	bool GetSoundData(FCSSoundID const& id, FVector& data) const;

	inline void RemoveSource(FName const& source)
	{ return iData.RemoveSource(source); };

	//TODO?
	CSIndicatorDelegates* RegisterIndicator(FCSRegisterArgs args);

	//TODO?
	void UnregisterIndicator(FCSSoundID const& id, UObject* widget);
	
	void Copy(CSTrackingManager const* manager);

private:
	CSIndicatorDelegates iDelegates;
	FCSTrackingData iData;
};