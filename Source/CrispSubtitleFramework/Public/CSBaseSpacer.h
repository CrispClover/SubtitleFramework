// Copyright Crisp Clover.

#pragma once

#include "CoreMinimal.h"
#include "CSBaseWidget.h"
#include "CSBaseSpacer.generated.h"

class USpacer;

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class CRISPSUBTITLEFRAMEWORK_API UCSBaseSpacer : public UCSBaseWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly, Category = "CrispSubtitles", meta = (BindWidgetOptional))
		USpacer* Spacer = nullptr;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CrispSubtitles|UI")//TODO
		void SetSize(FVector2D const& Size);
};
