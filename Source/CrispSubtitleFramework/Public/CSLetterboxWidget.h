// Copyright Crisp Clover.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CSUserSettings.h"
#include "CSLetterboxWidget.generated.h"

class UInvalidationBox;
class UBorder;
class UVerticalBox;
class UCSIndicatorWidget;

/*
 * 
 */
UCLASS(Blueprintable, Abstract)
class CRISPSUBTITLEFRAMEWORK_API UCSLetterboxWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void SynchronizeProperties() override;

public:
	UPROPERTY(BlueprintReadOnly, Category = "CrispSubtitles|UI", meta = (BindWidgetOptional))
		UInvalidationBox* InvalidationBox = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "CrispSubtitles|UI", meta = (BindWidgetOptional))
		UCSIndicatorWidget* Indicator = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "CrispSubtitles|UI", meta = (BindWidget))
		UBorder* Background = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "CrispSubtitles|UI", meta = (BindWidget))
		UVerticalBox* LineContainer = nullptr;

	UFUNCTION(BlueprintNativeEvent, Category = "CrispSubtitles|Broadcast")
		void ConstructFromSubtitle(FCrispSubtitle const& Subtitle, UCSUserSettings* Settings);

private:
	void ConstructFromSubtitle_Implementation(FCrispSubtitle const& subtitle, UCSUserSettings* settings);
};