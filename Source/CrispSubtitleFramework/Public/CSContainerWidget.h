// Copyright Crisp Clover.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CSContainerWidget.generated.h"

class UVerticalBox;
class UVerticalBoxSlot;
class UCSS_SubtitleGISS;

template <typename ChildType>
struct FCSChildWidgetData
{
	inline int32 rxFind(const int32 id)
		{ return IDs.Find(id); };

	inline int32 Num()
		{ return IDs.Num(); };

	inline void Add(const int32 id, ChildType* child, UVerticalBoxSlot* slot)
	{
		IDs.Add(id);
		Children.Add(child);
		Slots.Add(slot);
	};

	inline void Remove(const int32 id)
		{ iRemoveAt(rxFind(id));	};

	inline ChildType* rConsume(const int32 id)
	{
		const int32 rx = rxFind(id);

		if (rx == INDEX_NONE)
			return nullptr;
		
		ChildType* ptrCopy = Children[rx];
		iRemoveAt(rx);
		return ptrCopy;
	};

	TArray<ChildType*> Children = TArray<ChildType*>();
	TArray<UVerticalBoxSlot*> Slots = TArray<UVerticalBoxSlot*>();
	TArray<int32> IDs = TArray<int32>();

private:
	inline void iRemoveAt(const int32 index)
	{
		IDs.RemoveAt(index);
		Slots.RemoveAt(index);
		Children.RemoveAt(index);
	};
};

/**
 * 
 */
UCLASS(Abstract)
class CRISPSUBTITLEFRAMEWORK_API UCSContainerWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	UPROPERTY(BlueprintReadOnly, Category = "CrispSubtitles|UI", meta = (BindWidget))
		UVerticalBox* Container = nullptr;

protected:
	UCSS_SubtitleGISS* oCSS = nullptr;
};
