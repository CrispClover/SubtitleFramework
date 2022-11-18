// Copyright Crisp Clover.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Components/Widget.h"
#include "Widgets/Input/SComboBox.h"
#include "CSS_SubtitleGISS.h"
#include "CSUserSettingsSelectionWidget.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSelectionChanged, UCSUserSettings*, SelectedOption, ESelectInfo::Type, SelectionType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOpening);

/*
 * 
 */
UCLASS()
class CRISPSUBTITLEFRAMEWORK_API UCSUserSettingsSelectionWidget : public UWidget
{
	GENERATED_BODY()

public:
	UCSUserSettingsSelectionWidget();

	virtual void Construct();//Call on NativeConstruct

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CrispSubtitles|Data")
		TArray<UCSUserSettings*> SettingsOptions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CrispSubtitles|Data")
		FSlateFontInfo FontInfo;

	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles")
		void SetSelectedSettings(UCSUserSettings* Option);

	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles")
		UCSUserSettings* GetSelectedSettings() const;

	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles")
		bool IsOpen() const;
	
	TSharedRef<SWidget> GenerateOptionWidget(UCSUserSettings* Option);

	// Called when a new setting is selected.
	UPROPERTY(BlueprintAssignable, Category = Events)
		FSelectionChanged SelectionChangedEvent;

	// Called when the selection widget is opening
	UPROPERTY(BlueprintAssignable, Category = Events)
		FOpening OpeningEvent;

protected:
	UCSUserSettings* oSelectedSettings;

	UCSS_SubtitleGISS* oCSS = nullptr;

	virtual void oOnSettingsLoaded();

private:
	TSharedPtr<SComboBox<UCSUserSettings*>> iComboBox;
	TSharedPtr<SBox> iContentBox;

	void iOnSelectionChanged(UCSUserSettings* Item, ESelectInfo::Type SelectionType);

// Copied from ComboBoxKey.h || Copyright Epic Games, Inc. All Rights Reserved. -->
public:
	/* The combobox style. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Style, meta = (DisplayName = "Style"))
		FComboBoxStyle WidgetStyle;

	/* The item row style. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Style)
		FTableRowStyle ItemStyle;

	/* The foreground color to pass through the hierarchy. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Style, meta = (DesignerRebuild))
		FSlateColor ForegroundColor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Style)
		FMargin ContentPadding;

	/* The max height of the combobox list that opens */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Style, AdvancedDisplay)
		float MaxListHeight;

	/*
	 * When false, the down arrow is not generated and it is up to the API consumer
	 * to make their own visual hint that this is a drop down.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Style, AdvancedDisplay)
		bool bHasDownArrow;

	/*
	 * When false, directional keys will change the selection. When true, ComboBox
	 * must be activated and will only capture arrow input while activated.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Style, AdvancedDisplay)
		bool bEnableGamepadNavigationMode;

	/* When true, allows the combo box to receive keyboard focus */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Style)
		bool bIsFocusable;

public:

	//~ Begin UVisual Interface
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
	//~ End UVisual Interface

#if WITH_EDITOR
	virtual const FText GetPaletteCategory() override;
#endif

private:
	/* Called by slate when it needs to generate the widget in the content box */
	void GenerateContent();

	/* Called by slate when the underlying combobox is opening */
	void OnOpening();

	//~ Begin UWidget Interface
	virtual TSharedRef<SWidget> RebuildWidget() override;
	//~ End UWidget Interface

// <-- Copied from ComboBoxKey.h || Copyright Epic Games, Inc. All Rights Reserved.
};
