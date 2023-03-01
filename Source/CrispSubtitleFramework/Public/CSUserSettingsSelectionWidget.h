// Copyright Crisp Clover.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Components/Widget.h"
#include "Widgets/Input/SComboBox.h"
#include "Engine/AssetManager.h"
#include "CSProjectSettingFunctions.h"
#include "CSUserSettingsSelectionWidget.generated.h"

class UObjectLibrary;
class UCSS_SubtitleGISS;
class UCSUserSettings;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSelectionChanged, UCSUserSettings*, SelectedOption, ESelectInfo::Type, SelectionType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOpening);

/**
 * 
 */
UCLASS()
class CRISPSUBTITLEFRAMEWORK_API UCSUserSettingsSelectionWidget : public UWidget
{
	GENERATED_UCLASS_BODY()
		
protected:
	virtual void SynchronizeProperties() override;

public:
	//Called when a new setting is selected.
	UPROPERTY(BlueprintAssignable, Category = Events)
		FSelectionChanged SelectionChangedEvent;

	//Called when the selection widget is opening
	UPROPERTY(BlueprintAssignable, Category = Events)
		FOpening OpeningEvent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CrispSubtitles")
		TArray<UCSUserSettings*> SettingsOptions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Style")
		FSlateFontInfo FontInfo;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Style")
		FComboBoxStyle ComboboxStyle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Style")
		FTableRowStyle ItemRowStyle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Style", meta = (DesignerRebuild))
		FSlateColor ForegroundColor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Style")
		FMargin ContentPadding;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Style", AdvancedDisplay)
		float MaxDropdownHeight;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Style", AdvancedDisplay)
		bool bDisplayDropdownHintArrow;

	/**
	 * When false, directional keys will change the selection.
	 * When true, ComboBox must be activated to capture directional key input.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Navigation", AdvancedDisplay)
		bool bEnableGamepadNavigationMode;

	//Should the ComboBox receive keyboard focus?
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Navigation")
		bool bIsFocusable;

	//Synchronously loads user settings from the specified path.
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Settings")
		void LoadSettings();//TODO: remove?

	//Returns the list of loaded user settings. Loads the settings from DefaultLoadPath if no settings are currently loaded.
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles")
		TArray<UCSUserSettings*> GetSettingsList();

	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles")
		void SetSelectedSettings(UCSUserSettings* Option);

	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles")
		UCSUserSettings* GetSelectedSettings() const;

	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles")
		bool IsOpen() const;
	
	TSharedRef<SWidget> GenerateOptionWidget(UCSUserSettings* option);

protected:
	UCSS_SubtitleGISS* uCSS = nullptr;
	UCSUserSettings* oSelectedSettings;

	virtual void oOnSettingsLoaded();

private:
	typedef TMemFunPtrType<false, UCSUserSettingsSelectionWidget, void()>::Type SelectWidgetVFunction;

	//Returns false if all settings assets are already loaded.
	bool iLoadSettingsAsync(SelectWidgetVFunction function);
	void iOnSelectionChanged(UCSUserSettings* item, ESelectInfo::Type selectionType);
	void iGenerateContent();
	void iOnOpening();

	UPROPERTY()
		UObjectLibrary* uSettingsLibrary = nullptr;

	TSharedPtr<SComboBox<UCSUserSettings*>> iComboBox;
	TSharedPtr<SBox> iContentBox;

//Overrides
public:
	virtual void ReleaseSlateResources(bool releaseChildren) override;
	
#if WITH_EDITOR
	virtual inline const FText GetPaletteCategory() override
		{ return NSLOCTEXT("CrispSubtitles", "Subtitles", "Subtitles"); };
#endif

private:
	virtual TSharedRef<SWidget> RebuildWidget() override;
};
