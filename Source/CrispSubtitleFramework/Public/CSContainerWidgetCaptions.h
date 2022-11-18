// Copyright Crisp Clover.

#pragma once

#include "CoreMinimal.h"
#include "CSContainerWidget.h"
#include "CSContainerWidgetCaptions.generated.h"

class UCSCaptionWidget;

/*
 * 
 */
UCLASS(Blueprintable, Abstract)
class CRISPSUBTITLEFRAMEWORK_API UCSContainerWidgetCaptions : public UCSContainerWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CrispSubtitles|UI")
		TSubclassOf<UCSCaptionWidget> CaptionClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CrispSubtitles|UI")
		TSubclassOf<UCSCaptionWidget> IndicatorCaptionClass;

	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Events")
		UVerticalBoxSlot* GetSlot(const int32 ID);

	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Events")
		UCSCaptionWidget* GetCaptionWidget(const int32 ID);

	UFUNCTION(BlueprintNativeEvent, Category = "CrispSubtitles|Events")
		void OnCaptionReceived(FCrispCaption const& Caption);

	UFUNCTION(BlueprintNativeEvent, Category = "CrispSubtitles|Events")
		void OnDestroy(const int32 ID);

	UFUNCTION(BlueprintNativeEvent, Category = "CrispSubtitles|Events")
		void OnReconstruct(TArray<FCrispCaption> const& Captions);

private:
	FCSChildWidgetData<UCSCaptionWidget> iChildrenData = FCSChildWidgetData<UCSCaptionWidget>();
};
