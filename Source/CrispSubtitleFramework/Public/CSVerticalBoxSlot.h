// Copyright Crisp Clover.

#pragma once

#include "CoreMinimal.h"
#include "Components/PanelSlot.h"
#include "Layout/Margin.h"
#include "Components/SlateWrapperTypes.h"
#include "Widgets/SBoxPanel.h"
#include "CSVerticalBoxSlot.generated.h"

/**
 * 
 */
UCLASS()
class CRISPSUBTITLEFRAMEWORK_API UCSVerticalBoxSlot : public UPanelSlot
{	
	GENERATED_UCLASS_BODY()

#if WITH_EDITOR
public:
	virtual bool NudgeByDesigner(const FVector2D& nudgeDirection, const TOptional<int32>& gridSnapSize) override;
	virtual void SynchronizeFromTemplate(const UPanelSlot* const templateSlot) override;
#endif

public:
	virtual void SynchronizeProperties() override;
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Layout|Vertical Box Slot", meta = (DisplayAfter = "Padding"))
		FSlateChildSize Size;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Layout|Vertical Box Slot")
		FMargin Padding;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Layout|Vertical Box Slot")
		TEnumAsByte<EHorizontalAlignment> HorizontalAlignment;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Layout|Vertical Box Slot")
		TEnumAsByte<EVerticalAlignment> VerticalAlignment;

	UFUNCTION(BlueprintCallable, Category="Layout|Vertical Box Slot")
		void SetPadding(FMargin InPadding);

	UFUNCTION(BlueprintCallable, Category="Layout|Vertical Box Slot")
		void SetSize(FSlateChildSize InSize);

	UFUNCTION(BlueprintCallable, Category="Layout|Vertical Box Slot")
		void SetHorizontalAlignment(EHorizontalAlignment InHorizontalAlignment);

	UFUNCTION(BlueprintCallable, Category="Layout|Vertical Box Slot")
		void SetVerticalAlignment(EVerticalAlignment InVerticalAlignment);

	void AddSlot(TSharedRef<SVerticalBox> verticalBox);
	void InsertSlot(TSharedRef<SVerticalBox> verticalBox, const int32 x);

private:
	SVerticalBox::FSlot* iSlot;
};
