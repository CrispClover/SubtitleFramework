// Copyright Crisp Clover.

#include "CSUserSettingsSelectionWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Styling/UMGCoreStyle.h"
#include "Engine/ObjectLibrary.h"
#include "CSS_SubtitleGISS.h"
#include "CSUserSettings.h"

void UCSUserSettingsSelectionWidget::SynchronizeProperties()
{
	Super::SynchronizeProperties();

#if WITH_EDITOR
	if (IsDesignTime())
		return;
#endif

	uCSS = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UCSS_SubtitleGISS>();

	const bool settingsNeedLoading = iLoadSettingsAsync(DefaultLoadPath, &UCSUserSettingsSelectionWidget::oOnSettingsLoaded);

	if (!settingsNeedLoading)
		oOnSettingsLoaded();
}

UCSUserSettingsSelectionWidget::UCSUserSettingsSelectionWidget()
{
	if (!IsRunningDedicatedServer())
	{
		ComboboxStyle = FUMGCoreStyle::Get().GetWidgetStyle<FComboBoxStyle>("ComboBox");
		ItemRowStyle = FUMGCoreStyle::Get().GetWidgetStyle<FTableRowStyle>("TableView.Row");
	}

	ContentPadding = FMargin(4.0, 2.0);
	ForegroundColor = ItemRowStyle.TextColor;
	MaxDropdownHeight = 450.0f;
	bDisplayDropdownHintArrow = true;
	bEnableGamepadNavigationMode = true;
	bIsFocusable = true;

#if WITH_EDITOR
	oSelectedSettings = UCSProjectSettingFunctions::GetDefaultSettings();
	SettingsOptions.Add(oSelectedSettings);
#endif
}

void UCSUserSettingsSelectionWidget::LoadSettings(FString const& path)
{
	if (!uSettingsLibrary)
		uSettingsLibrary = UObjectLibrary::CreateLibrary(UCSUserSettings::StaticClass(), false, GIsEditor);

	uSettingsLibrary->LoadAssetDataFromPath(path);
	uSettingsLibrary->LoadAssetsFromAssetData();
}

TArray<UCSUserSettings*> UCSUserSettingsSelectionWidget::GetSettingsList()
{
	if (!uSettingsLibrary)
		LoadSettings(DefaultLoadPath);

	TArray<FAssetData> aDataList;
	uSettingsLibrary->GetAssetDataList(aDataList);

	TArray<UCSUserSettings*> settingsList;
	settingsList.Reserve(aDataList.Num());

	for (FAssetData aData : aDataList)
		settingsList.Add(Cast<UCSUserSettings>(aData.GetAsset()));

	return settingsList;
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

TSharedRef<SWidget> UCSUserSettingsSelectionWidget::GenerateOptionWidget(UCSUserSettings* option)
{
	return SNew(STextBlock)
		.Text(option->DisplayName)
		.Font(FontInfo);
}

void UCSUserSettingsSelectionWidget::oOnSettingsLoaded()
{
	SettingsOptions = GetSettingsList();

	if (iComboBox)
		iComboBox->RefreshOptions();

	SetSelectedSettings(uCSS->GetCurrentSettings());
}

bool UCSUserSettingsSelectionWidget::iLoadSettingsAsync(FString const& path, SelectWidgetVFunction function)
{
	if (!uSettingsLibrary)
		uSettingsLibrary = UObjectLibrary::CreateLibrary(UCSUserSettings::StaticClass(), false, GIsEditor);

	const int32 numSettings = uSettingsLibrary->LoadAssetDataFromPath(path);

	TArray<FSoftObjectPath> settingsPaths;
	settingsPaths.Reserve(numSettings);

	TArray<FAssetData> assetDataList;
	uSettingsLibrary->GetAssetDataList(assetDataList);

	for (FAssetData assetData : assetDataList)
		if (!assetData.IsAssetLoaded())
			settingsPaths.Add(assetData.ToSoftObjectPath());

	if (!settingsPaths.Num())
		return false;

	UAssetManager::GetStreamableManager().RequestAsyncLoad
	(
		  settingsPaths
		, FStreamableDelegate::CreateUObject(this, function)
	);

	return true;
}

void UCSUserSettingsSelectionWidget::iOnSelectionChanged(UCSUserSettings* option, ESelectInfo::Type selectionType)
{
	if (oSelectedSettings == option)
		return;

	oSelectedSettings = option;

	if (!IsDesignTime())
		SelectionChangedEvent.Broadcast(option, selectionType);

	iGenerateContent();
}

void UCSUserSettingsSelectionWidget::iGenerateContent()
{
	iContentBox->SetContent
	(
		SNew(STextBlock)
		.Text(oSelectedSettings->DisplayName)
		.Font(FontInfo)
	);
}

void UCSUserSettingsSelectionWidget::iOnOpening()
{
	if (!IsDesignTime())
		OpeningEvent.Broadcast();
}

void UCSUserSettingsSelectionWidget::ReleaseSlateResources(bool releaseChildren)
{
	Super::ReleaseSlateResources(releaseChildren);

	iComboBox.Reset();
	iContentBox.Reset();
}

#if WITH_EDITOR
const FText UCSUserSettingsSelectionWidget::GetPaletteCategory()
{
	return NSLOCTEXT("CrispSubtitles", "CSInput", "CSInput");
}
#endif

TSharedRef<SWidget> UCSUserSettingsSelectionWidget::RebuildWidget()
{
	iComboBox =
		SNew(SComboBox<UCSUserSettings*>)
		.OptionsSource(&SettingsOptions)
		.InitiallySelectedItem(oSelectedSettings)
		.OnGenerateWidget_UObject(this, &UCSUserSettingsSelectionWidget::GenerateOptionWidget)
		.OnSelectionChanged_UObject(this, &UCSUserSettingsSelectionWidget::iOnSelectionChanged)
		.OnComboBoxOpening_UObject(this, &UCSUserSettingsSelectionWidget::iOnOpening)
		.ComboBoxStyle(&ComboboxStyle)
		.ItemStyle(&ItemRowStyle)
		.ForegroundColor(ForegroundColor)
		.ContentPadding(ContentPadding)
		.MaxListHeight(MaxDropdownHeight)
		.HasDownArrow(bDisplayDropdownHintArrow)
		.EnableGamepadNavigationMode(bEnableGamepadNavigationMode)
		.IsFocusable(bIsFocusable)
		[
			SAssignNew(iContentBox, SBox)
		];

	iGenerateContent();

	return iComboBox.ToSharedRef();
}