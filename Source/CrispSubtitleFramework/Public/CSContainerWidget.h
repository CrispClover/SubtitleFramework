// Copyright Crisp Clover.

#pragma once

#include "CoreMinimal.h"
#include "CSBaseWidget.h"
#include "CSContainerWidget.generated.h"

class UCSVerticalBox;
class UCSUserSettings;
class UCSS_SubtitleGISS;

/**
 * 
 */
UCLASS(Abstract)
class CRISPSUBTITLEFRAMEWORK_API UCSContainerWidget : public UCSBaseWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category = "CrispSubtitles|UI", meta = (BindWidgetOptional))
		UCSVerticalBox* Container = nullptr;

protected:
	UCSS_SubtitleGISS* uCSS = nullptr;
	UCSUserSettings const* uSettings = nullptr;

	inline float otNow() const
	{
		if (UWorld* world = GetWorld())
			return world->GetRealTimeSeconds();
		else
			return 0.f;
	};
};
