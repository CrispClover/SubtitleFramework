// Copyright Crisp Clover.

#pragma once

#include "CoreMinimal.h"
#include "Components/VerticalBox.h"
#include "CSVerticalBox.generated.h"

class UCSVerticalBoxSlot;
struct FCSSpacerInfo;

//Bundle to calculate flicker protection.
struct CSFlickerData
{
	CSFlickerData() = delete;

	CSFlickerData(const float tNow)
		: itChanged(tNow)
		, ifChanged(GFrameNumber)
	{};

	inline bool IsAvailable(const float tNow, const float dtGap) const
		{ return (tNow > itChanged + dtGap) || (ifChanged == GFrameNumber); };

	inline float dtMissing(const float tNow, const float dtGap) const
	{
		if (ifChanged == GFrameNumber)
			return -1.f;
		else
			return itChanged + dtGap - tNow;
	};

private:
	float itChanged;
	uint32 ifChanged;
};

/**
 * 
 */
UCLASS()
class CRISPSUBTITLEFRAMEWORK_API UCSVerticalBox : public UPanelWidget
{
	GENERATED_BODY()

#if WITH_EDITOR
public:
	virtual inline const FText GetPaletteCategory() override
		{ return NSLOCTEXT("CrispSubtitles", "Subtitles", "Subtitles"); };
#endif

public:
	UCSVerticalBoxSlot* rFindSlot(const int32 id);
	UWidget* rFindChild(const int32 id);

	UCSVerticalBoxSlot* FindOrAddSlot(UWidget* newChild, const float tNow, const float dtGap = 0.16f, const int32 id = INDEX_NONE);
	UCSVerticalBoxSlot* AddVacantSlot(FCSSpacerInfo const& info, const float currentTime);

	float dtTryVacate(const int32 id, FCSSpacerInfo const& info, const float tNow, const float dtGap);

	void ClearVacant(const float tNow, const float dtGap);
	void ClearExcessVacant(const float tNow, const float dtGap);

protected:
	virtual UClass* GetSlotClass() const override;
	virtual void OnSlotAdded(UPanelSlot* Slot) override;
	virtual void OnSlotRemoved(UPanelSlot* Slot) override;
	virtual void ReleaseSlateResources(bool releaseChildren) override;
	virtual TSharedRef<SWidget> RebuildWidget() override;

private:
	TSharedPtr<class SVerticalBox> iVerticalBox;
	TArray<CSFlickerData> iFlickerData;
	TArray<int32> iIDs;
	TArray<bool> iVacant;

	float udtTryVacate(int32 x, UWidget* spacer, const float tNow, const float dtGap);
	void uReplaceChildAt(const int32 x, UWidget* newChild);

	void iAddData(CSFlickerData const& flickerData, const bool vacant, const int32 id = INDEX_NONE);
	void uSetData(const int32 x, CSFlickerData const& flickerData, const bool vacant, const int32 id = INDEX_NONE);
	void uRemoveData(const int32 x);
};
