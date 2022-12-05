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
	//(Optional) The widget that lets the user select pre-defined settings.
	UPROPERTY(BlueprintReadOnly, Category = "CrispSubtitles|UI", meta = (BindWidgetOptional))
		UCSUserSettingsSelectionWidget* SettingsSelector;
	
	//A preview of a subtitle with the current settings applied.
	UPROPERTY(BlueprintReadOnly, Category = "CrispSubtitles|UI", meta = (BindWidget))
		UCSLetterboxWidget* SubtitlePreview;
	
	//(Optional) A preview of a caption with the current settings applied.
	UPROPERTY(BlueprintReadOnly, Category = "CrispSubtitles|UI", meta = (BindWidgetOptional))
		UCSCaptionWidget* CaptionPreview;

	//The settings currently being previewed.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CrispSubtitles|Data")
		UCSUserSettings* CurrentSettings;

	UFUNCTION(BlueprintNativeEvent, Category = "CrispSubtitles|Events")
		void OnSettingsSelected(UCSUserSettings* SelectedSettings, ESelectInfo::Type SelectionType);

	UFUNCTION(BlueprintNativeEvent, Category = "CrispSubtitles|UI")
		void ReconstructExample();

	//Saves the currently selected settings with the subtitle subsystem.
	UFUNCTION(BlueprintNativeEvent, Category = "CrispSubtitles|UI")
		void Save();

protected:
	virtual void OnSettingsSelected_Implementation(UCSUserSettings* SelectedSettings, ESelectInfo::Type SelectionType);
	virtual void ReconstructExample_Implementation();
	virtual void Save_Implementation();

	UCSS_SubtitleGISS* uCSS = nullptr;
};
