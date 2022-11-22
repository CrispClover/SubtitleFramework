// Copyright Crisp Clover.

#pragma once

#include "CoreMinimal.h"
#include "CSContainerWidget.h"
#include "CSContainerWidgetSubtitles.generated.h"

class UCSLetterboxWidget;

/**
 * 
 */
UCLASS(Blueprintable, Abstract)
class CRISPSUBTITLEFRAMEWORK_API UCSContainerWidgetSubtitles : public UCSContainerWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

public:
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Events")
		UVerticalBoxSlot* GetSlot(const int32 ID);

	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Events")
		UCSLetterboxWidget* GetLetterbox(const int32 ID);
	
	UFUNCTION(BlueprintNativeEvent, Category = "CrispSubtitles|Events")
		void OnSubtitleReceived(FCrispSubtitle const& subtitle);

	UFUNCTION(BlueprintNativeEvent, Category = "CrispSubtitles|Events")
		void OnDestroy(const int32 ID);

	UFUNCTION(BlueprintNativeEvent, Category = "CrispSubtitles|Events")
		void OnReconstruct(TArray<FCrispSubtitle> const& Subtitles);

private:
	FCSChildWidgetData<UCSLetterboxWidget> iChildrenData = FCSChildWidgetData<UCSLetterboxWidget>();
};
