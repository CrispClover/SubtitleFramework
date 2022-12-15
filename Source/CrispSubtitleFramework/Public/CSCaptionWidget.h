// Copyright Crisp Clover.

#pragma once

#include "CoreMinimal.h"
#include "CSBaseWidget.h"
#include "CSCoreLibrary.h"
#include "CSUILibrary.h"
#include "CSCaptionWidget.generated.h"

class UBorder;
class UInvalidationBox;
class UCSIndicatorWidget;
class UTextBlock;
class UCSUserSettings;

/**
 * 
 */
UCLASS(Blueprintable, Abstract)
class CRISPSUBTITLEFRAMEWORK_API UCSCaptionWidget : public UCSBaseWidget
{
	GENERATED_BODY()
	
#if WITH_EDITOR
protected:
	virtual void eConstructExample(FVector2D const& size) override;
#endif

public:
	UPROPERTY(BlueprintReadOnly, Category = "CrispSubtitles|UI", meta = (BindWidget))
		UBorder* Background = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "CrispSubtitles|UI", meta = (BindWidget))
		UTextBlock* Text = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "CrispSubtitles|UI", meta = (BindWidgetOptional))
		UCSIndicatorWidget* Indicator = nullptr;

	//BUG?: InvalidationBox' children appear disabled after adding subtitles to the container on the same canvas.
	/*UPROPERTY(BlueprintReadOnly, Category = "CrispSubtitles|UI", meta = (BindWidgetOptional))
		UInvalidationBox* InvalidationBox = nullptr;*/

	UFUNCTION(BlueprintNativeEvent, Category = "CrispSubtitles|Broadcast")
		void ConstructFromCaption(FCrispCaption const& Caption, FCSCaptionStyle const& Style);
};
