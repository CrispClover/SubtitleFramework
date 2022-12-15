// Copyright Crisp Clover.

#pragma once

#include "CoreMinimal.h"
#include "CSContainerWidget.h"
#include "CSContainerWidgetSubtitles.generated.h"

class UCSLetterboxWidget;
class UVerticalBoxSlot;

/**
 * 
 */
UCLASS(Blueprintable, Abstract)
class CRISPSUBTITLEFRAMEWORK_API UCSContainerWidgetSubtitles : public UCSContainerWidget
{
	GENERATED_BODY()

#if WITH_EDITOR
protected:
	TArray<UCSLetterboxWidget*> eExamples = TArray<UCSLetterboxWidget*>();
	virtual void eConstructExample(FVector2D const& size) override;
#endif

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

public:

	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Events")
		UCSVerticalBoxSlot* GetSlot(const int32 ID);

	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Events")
		UCSLetterboxWidget* GetLetterbox(const int32 ID);
	
	UFUNCTION(BlueprintNativeEvent, Category = "CrispSubtitles|Events")
		void OnSubtitleReceived(FCrispSubtitle const& subtitle);

	UFUNCTION(BlueprintNativeEvent, Category = "CrispSubtitles|Events")
		void OnDestroy(const int32 ID);

	UFUNCTION(BlueprintNativeEvent, Category = "CrispSubtitles|Events")
		void OnReconstruct(TArray<FCrispSubtitle> const& Subtitles, UCSUserSettings* Settings);

	void OnSubtitleReceived_Implementation(FCrispSubtitle const& subtitle);
	void OnDestroy_Implementation(const int32 id);
	void OnReconstruct_Implementation(TArray<FCrispSubtitle> const& subtitles, UCSUserSettings* settings);

	void SetSettings(UCSUserSettings* settings);
};
