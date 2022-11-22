// Copyright Crisp Clover.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CSLibrary.h"
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
class CRISPSUBTITLEFRAMEWORK_API UCSCaptionWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void SynchronizeProperties() override;

public:
	UPROPERTY(BlueprintReadOnly, Category = "CrispSubtitles|UI", meta = (BindWidget))
		UBorder* Background = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "CrispSubtitles|UI", meta = (BindWidgetOptional))
		UCSIndicatorWidget* Indicator = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "CrispSubtitles|UI", meta = (BindWidgetOptional))
		UInvalidationBox* InvalidationBox = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "CrispSubtitles|UI", meta = (BindWidget))
		UTextBlock* Text = nullptr;

	UFUNCTION(BlueprintNativeEvent, Category = "CrispSubtitles|Broadcast")
		void ConstructFromCaption(FCrispCaption const& Caption, UCSUserSettings* settings);
};
