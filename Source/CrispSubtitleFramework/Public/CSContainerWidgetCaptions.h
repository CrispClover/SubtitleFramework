// Copyright Crisp Clover.

#pragma once

#include "CoreMinimal.h"
#include "CSContainerWidget.h"
#include "CSContainerWidgetCaptions.generated.h"

class UCSCaptionWidget;
class UCSVerticalBoxSlot;

/**
 * 
 */
UCLASS(Blueprintable, Abstract)
class CRISPSUBTITLEFRAMEWORK_API UCSContainerWidgetCaptions : public UCSContainerWidget
{
	GENERATED_BODY()

#if WITH_EDITOR
protected:
	TArray<UCSCaptionWidget*> eExamples = TArray<UCSCaptionWidget*>();
	virtual void eConstructExample(FVector2D const& size) override;
#endif

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

public:
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Events")
		UCSVerticalBoxSlot* GetSlot(const int32 ID);

	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Events")
		UCSCaptionWidget* GetCaptionWidget(const int32 ID);

	UFUNCTION(BlueprintNativeEvent, Category = "CrispSubtitles|Events")
		void OnCaptionReceived(FCrispCaption const& Caption);

	UFUNCTION(BlueprintNativeEvent, Category = "CrispSubtitles|Events")
		void OnDestroy(const int32 ID);

	UFUNCTION(BlueprintNativeEvent, Category = "CrispSubtitles|Events")
		void OnReconstruct(TArray<FCrispCaption> const& Captions, UCSUserSettings* Settings);

	void OnCaptionReceived_Implementation(FCrispCaption const& caption);
	void OnDestroy_Implementation(const int32 id);
	void OnReconstruct_Implementation(TArray<FCrispCaption> const& captions, UCSUserSettings* settings);
};
