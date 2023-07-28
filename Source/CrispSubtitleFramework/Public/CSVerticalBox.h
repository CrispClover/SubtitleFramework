// Copyright Crisp Clover.

#pragma once

#include "CoreMinimal.h"
#include "Components/VerticalBox.h"
#include "CSVerticalBox.generated.h"

class UCSVerticalBoxSlot;

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
	
	UCSVerticalBoxSlot* AddDesignChild(UWidget* child);
#endif

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Layout")
		bool bAddToTop = false;

	UCSVerticalBoxSlot* uFindSlot(const int32 id);
	UWidget* uFindChild(const int32 id);

	UCSVerticalBoxSlot* FindOrAddSlot(UWidget* newChild, const float tNow, const float dtGap = 0.16f, const int32 id = INDEX_NONE);
	UCSVerticalBoxSlot* AddVacantSlot(TSubclassOf<UCSBaseSpacer> spacerClass, FVector2D size, const float currentTime);

	float dtTryVacate(const int32 id, TSubclassOf<UCSBaseSpacer> spacerClass, const float tNow, const float dtGap);

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

	float vdtTryVacate(int32 x, UWidget* spacer, const float tNow, const float dtGap);
	void vReplaceChildAt(const int32 x, UWidget* newChild);
	UPanelSlot* iAddChild(UWidget* child);
	UCSVerticalBoxSlot* iAddChild(UWidget* child, const float tNow, const bool vacant, const int32 id = INDEX_NONE);
	
	void iAddData(CSFlickerData const& flickerData, const bool vacant, const int32 id = INDEX_NONE);
	void vInsertData(const int32 x, CSFlickerData const& flickerData, const bool vacant, const int32 id = INDEX_NONE);
	void vSetData(const int32 x, CSFlickerData const& flickerData, const bool vacant, const int32 id = INDEX_NONE);
	void vRemoveData(const int32 x);
};
