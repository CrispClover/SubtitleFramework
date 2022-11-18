// Copyright Crisp Clover. 

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CSUserSettingsWidget.generated.h"

class UCSUserSettingsSelectionWidget;
class UCSLetterboxWidget;
class UCSUserSettings;
class UCSS_SubtitleGISS;

/**
 * 
 */
UCLASS(Blueprintable, Abstract)
class CRISPSUBTITLEFRAMEWORK_API UCSUserSettingsWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void SynchronizeProperties() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

public:
	UPROPERTY(BlueprintReadOnly, Category = "CrispSubtitles|UI", meta = (BindWidgetOptional))
		UCSUserSettingsSelectionWidget* SettingsSelector;

	UPROPERTY(BlueprintReadOnly, Category = "CrispSubtitles|UI", meta = (BindWidget))
		UCSLetterboxWidget* SubtitlePreview;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CrispSubtitles|Data")
		UCSUserSettings* CurrentSettings;

	UFUNCTION(BlueprintNativeEvent, Category = "CrispSubtitles|Events")
		void OnSettingsSelected(UCSUserSettings* SelectedSettings, ESelectInfo::Type SelectionType);

	UFUNCTION(BlueprintNativeEvent, Category = "CrispSubtitles|UI")
		void ReconstructExample();

	UFUNCTION(BlueprintNativeEvent, Category = "CrispSubtitles|UI")
		void Save();

protected:
	virtual void OnSettingsSelected_Implementation(UCSUserSettings* SelectedSettings, ESelectInfo::Type SelectionType);
	virtual void ReconstructExample_Implementation();
	virtual void Save_Implementation();

	UCSS_SubtitleGISS* oCSS = nullptr;
};
