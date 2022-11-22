// Copyright Crisp Clover.

#include "CSUserSettingsSelectionWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Styling/UMGCoreStyle.h"
//#include "UObject/ConstructorHelpers.h" TODO->
//#include "Widgets/Layout/SBox.h"
//#include "Widgets/Text/STextBlock.h"
//#include "Widgets/SNullWidget.h"
#include "CSS_SubtitleGISS.h"
#include "CSUserSettings.h"

UCSUserSettingsSelectionWidget::UCSUserSettingsSelectionWidget()
{
	if (!IsRunningDedicatedServer())
	{
		WidgetStyle = FUMGCoreStyle::Get().GetWidgetStyle<FComboBoxStyle>("ComboBox");
		ItemStyle = FUMGCoreStyle::Get().GetWidgetStyle<FTableRowStyle>("TableView.Row");
	}

	ContentPadding = FMargin(4.0, 2.0);
	ForegroundColor = ItemStyle.TextColor;
	MaxListHeight = 450.0f;
	bHasDownArrow = true;
	bEnableGamepadNavigationMode = true;
	bIsFocusable = true;

	oSelectedSettings = UCSProjectSettingFunctions::GetDefaultSettings();
	SettingsOptions.Add(oSelectedSettings);
}

void UCSUserSettingsSelectionWidget::Construct()
{
	oCSS = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UCSS_SubtitleGISS>();

	const bool settingsNeedLoading = oCSS->LoadSettingsAsync(FStreamableDelegate::CreateUObject(this, &UCSUserSettingsSelectionWidget::oOnSettingsLoaded));

	if (!settingsNeedLoading)
		oOnSettingsLoaded();
}

void UCSUserSettingsSelectionWidget::oOnSettingsLoaded()
{
	SettingsOptions = oCSS->GetSettingsList();

	if (iComboBox)
		iComboBox->RefreshOptions();

	SetSelectedSettings(oCSS->GetCurrentSettings());
}

void UCSUserSettingsSelectionWidget::GenerateContent()
{
	iContentBox->SetContent
	(
		SNew(STextBlock)
		.Text(oSelectedSettings->DisplayName)
		.Font(FontInfo)
	);
}

TSharedRef<SWidget> UCSUserSettingsSelectionWidget::GenerateOptionWidget(UCSUserSettings* option)
{
	return SNew(STextBlock)
		.Text(option->DisplayName)
		.Font(FontInfo);
}

void UCSUserSettingsSelectionWidget::iOnSelectionChanged(UCSUserSettings* option, ESelectInfo::Type selectionType)
{
	if (oSelectedSettings == option)
		return;

	oSelectedSettings = option;

	if (!IsDesignTime())
		SelectionChangedEvent.Broadcast(option, selectionType);

	GenerateContent();
}

void UCSUserSettingsSelectionWidget::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	iComboBox.Reset();
	iContentBox.Reset();
}

TSharedRef<SWidget> UCSUserSettingsSelectionWidget::RebuildWidget()
{
	iComboBox =
		SNew(SComboBox<UCSUserSettings*>)
		.OptionsSource(&SettingsOptions)
		.InitiallySelectedItem(oSelectedSettings)
		.OnGenerateWidget_UObject(this, &UCSUserSettingsSelectionWidget::GenerateOptionWidget)
		.OnSelectionChanged_UObject(this, &UCSUserSettingsSelectionWidget::iOnSelectionChanged)
		.OnComboBoxOpening_UObject(this, &UCSUserSettingsSelectionWidget::OnOpening)
		.ComboBoxStyle(&WidgetStyle)
		.ItemStyle(&ItemStyle)
		.ForegroundColor(ForegroundColor)
		.ContentPadding(ContentPadding)
		.MaxListHeight(MaxListHeight)
		.HasDownArrow(bHasDownArrow)
		.EnableGamepadNavigationMode(bEnableGamepadNavigationMode)
		.IsFocusable(bIsFocusable)
		[
			SAssignNew(iContentBox, SBox)
		];

	GenerateContent();

	return iComboBox.ToSharedRef();
}

void UCSUserSettingsSelectionWidget::SetSelectedSettings(UCSUserSettings* option)
{
	if (oSelectedSettings == option)
		return;

	if (iComboBox)
		iComboBox->SetSelectedItem(option);
	else
		oSelectedSettings = option;
}

UCSUserSettings* UCSUserSettingsSelectionWidget::GetSelectedSettings() const
{
	return oSelectedSettings;
}

bool UCSUserSettingsSelectionWidget::IsOpen() const
{
	return iComboBox && iComboBox->IsOpen();
}

void UCSUserSettingsSelectionWidget::OnOpening()
{
	if (!IsDesignTime())
		OpeningEvent.Broadcast();
}

#if WITH_EDITOR
const FText UCSUserSettingsSelectionWidget::GetPaletteCategory()
{
	return NSLOCTEXT("CrispSubtitles", "CSInput", "CSInput");
}
#endif
