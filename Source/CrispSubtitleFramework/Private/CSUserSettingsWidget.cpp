// Copyright Crisp Clover.

#include "CSUserSettingsWidget.h"
#include "CSS_SubtitleGISS.h"
#include "CSUserSettingsSelectionWidget.h"
#include "CSLetterboxWidget.h"
#include "Kismet/GameplayStatics.h"

void UCSUserSettingsWidget::SynchronizeProperties()
{
	Super::SynchronizeProperties();

#if WITH_EDITOR
	if (!IsDesignTime() || !SubtitlePreview)
		return;

	CurrentSettings = UCSProjectSettingFunctions::GetDefaultSettings();

	ReconstructExample();
#endif
}

void UCSUserSettingsWidget::NativeConstruct()
{
	oCSS = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UCSS_SubtitleGISS>();

	CurrentSettings = oCSS->GetCurrentSettings();

	if (SettingsSelector)
		SettingsSelector->SelectionChangedEvent.AddDynamic(this, &UCSUserSettingsWidget::OnSettingsSelected);
	
	if (ULocalPlayer* player = GetOwningLocalPlayer())
		CurrentSettings->RecalculateLayout(player->ViewportClient);

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

	if (ULocalPlayer* player = GetOwningLocalPlayer())
		CurrentSettings->RecalculateLayout(player->ViewportClient);

	ReconstructExample();
}

void UCSUserSettingsWidget::ReconstructExample_Implementation()
{
	const FCrispSubtitle sub = UCSProjectSettingFunctions::GetExampleSubtitle(CurrentSettings);
	SubtitlePreview->ConstructFromSubtitle(sub, UCSUILibrary::GetLetterboxStyle(CurrentSettings, sub.Speaker));
}

void UCSUserSettingsWidget::Save_Implementation()
{
	oCSS->SetSettings(CurrentSettings);
}
