// Copyright Crisp Clover.

#include "CSUserSettingsWidget.h"
#include "CSS_SubtitleGISS.h"
#include "CSUserSettingsSelectionWidget.h"
#include "CSLetterboxWidget.h"
#include "Kismet/GameplayStatics.h"

void UCSUserSettingsWidget::SynchronizeProperties()
{
	CurrentSettings = UCSProjectSettingFunctions::GetDefaultSettings();

	if (!SubtitlePreview)
		return;

	ReconstructExample();

	Super::SynchronizeProperties();
}

void UCSUserSettingsWidget::NativeConstruct()
{
	oCSS = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UCSS_SubtitleGISS>();

	CurrentSettings = oCSS->GetCurrentSettings();

	if (SettingsSelector)
	{
		SettingsSelector->Construct();
		SettingsSelector->SelectionChangedEvent.AddDynamic(this, &UCSUserSettingsWidget::OnSettingsSelected);
		//TODO?
	}

	ReconstructExample();

	Super::NativeConstruct();
}

void UCSUserSettingsWidget::NativeDestruct()
{
	SubtitlePreview->RemoveFromParent();
	Super::NativeDestruct();
}

void UCSUserSettingsWidget::OnSettingsSelected_Implementation(UCSUserSettings* selectedSettings, ESelectInfo::Type selectionType)
{
	CurrentSettings = selectedSettings;
	ReconstructExample();
}

void UCSUserSettingsWidget::ReconstructExample_Implementation()
{
	SubtitlePreview->ConstructFromSubtitle(UCSProjectSettingFunctions::GetExampleSubtitle(CurrentSettings), CurrentSettings);
}

void UCSUserSettingsWidget::Save_Implementation()
{
	oCSS->SetSettings(CurrentSettings);
}
