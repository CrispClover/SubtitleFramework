// Copyright Crisp Clover.

#pragma once

#include "CoreMinimal.h"
#include "CSBaseWidget.h"
#include "CSUserSettings.h"
#include "CSUILibrary.h"
#include "CSLetterboxWidget.generated.h"

class UInvalidationBox;
class UBorder;
class UVerticalBox;
class UCSIndicatorWidget;

/**
 * 
 */
UCLASS(Blueprintable, Abstract)
class CRISPSUBTITLEFRAMEWORK_API UCSLetterboxWidget : public UCSBaseWidget
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
		UVerticalBox* LineContainer = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "CrispSubtitles|UI", meta = (BindWidgetOptional))
		UInvalidationBox* InvalidationBox = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "CrispSubtitles|UI", meta = (BindWidgetOptional))
		UCSIndicatorWidget* Indicator = nullptr;

	UFUNCTION(BlueprintNativeEvent, Category = "CrispSubtitles|Broadcast")
		void ConstructFromSubtitle(FCrispSubtitle const& Subtitle, FCSLetterboxStyle const& Style);

private:
	void ConstructFromSubtitle_Implementation(FCrispSubtitle const& subtitle, FCSLetterboxStyle const& style);
};
