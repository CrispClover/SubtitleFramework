// Copyright Crisp Clover.

#include "CSUserSettingsWidget.h"
#include "CSS_SubtitleGISS.h"
#include "CSUserSettings.h"
#include "CSUserSettingsSelectionWidget.h"
#include "CSContainerWidgetSubtitles.h"
#include "CSContainerWidgetCaptions.h"
#include "Kismet/GameplayStatics.h"

#if WITH_EDITOR
void UCSUserSettingsWidget::eConstructExample(FVector2D const& size)
{
	Super::eConstructExample(size);

	if (!SubtitlePreview)
		return;

	CurrentSettings = UCSProjectSettingFunctions::GetDesignSettings(size);
	ReconstructExamples();
}
#endif

void UCSUserSettingsWidget::NativeConstruct()
{
	uCSS = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UCSS_SubtitleGISS>();

	CurrentSettings = uCSS->GetCurrentSettings();

	if (SettingsSelector)
		SettingsSelector->SelectionChangedEvent.AddDynamic(this, &UCSUserSettingsWidget::OnSettingsSelected);
	
	if (ULocalPlayer* player = GetOwningLocalPlayer())
		CurrentSettings->RecalculateLayout(player->ViewportClient);

	ReconstructExamples();

	Super::NativeConstruct();
}

void UCSUserSettingsWidget::NativeDestruct()
{
	SubtitlePreview->RemoveFromParent();

	if (CaptionPreview)
		CaptionPreview->RemoveFromParent();

	Super::NativeDestruct();
}

void UCSUserSettingsWidget::OnSettingsSelected_Implementation(UCSUserSettings* selectedSettings, ESelectInfo::Type selectionType)
{
	if (!selectedSettings)
		return;

	CurrentSettings = selectedSettings;

	if (ULocalPlayer* player = GetOwningLocalPlayer())
		CurrentSettings->RecalculateLayout(player->ViewportClient);

	ReconstructExamples();
}

void UCSUserSettingsWidget::ReconstructExamples_Implementation()
{
	TArray<FCrispSubtitle> const& subtitles = UCSProjectSettingFunctions::GetExampleSubtitles(CurrentSettings);
	SubtitlePreview->OnReconstruct(subtitles, CurrentSettings);

	if (!CaptionPreview)
		return;
	
	TArray<FCrispCaption> const& captions = UCSProjectSettingFunctions::GetExampleCaptions();
	CaptionPreview->OnReconstruct(captions, CurrentSettings);
}

void UCSUserSettingsWidget::Save_Implementation()
{
	uCSS->SetSettings(CurrentSettings);
}
