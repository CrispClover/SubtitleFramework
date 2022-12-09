// Copyright Crisp Clover.

#include "CSUserSettingsWidget.h"
#include "CSS_SubtitleGISS.h"
#include "CSUserSettingsSelectionWidget.h"
#include "CSLetterboxWidget.h"
#include "CSCaptionWidget.h"
#include "Kismet/GameplayStatics.h"

#if WITH_EDITOR
void UCSUserSettingsWidget::eConstructExample(FVector2D const& size)
{
	Super::eConstructExample(size);

	if (!SubtitlePreview)
		return;

	CurrentSettings = UCSProjectSettingFunctions::GetDesignSettings(size);
	ReconstructExample();
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

	ReconstructExample();

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

	ReconstructExample();
}

void UCSUserSettingsWidget::ReconstructExample_Implementation()
{
	const FCrispSubtitle sub = UCSProjectSettingFunctions::GetExampleSubtitle(CurrentSettings);
	SubtitlePreview->ConstructFromSubtitle(sub, UCSUILibrary::GetLetterboxStyle(CurrentSettings, sub.Speaker, sub.IsIndirectSpeech()));

	if (!CaptionPreview)
		return;

	FCrispCaption const& cap = UCSProjectSettingFunctions::GetExampleCaption();
	CaptionPreview->ConstructFromCaption(cap, UCSUILibrary::GetCaptionStyle(CurrentSettings, cap.SoundID.Source));
}

void UCSUserSettingsWidget::Save_Implementation()
{
	uCSS->SetSettings(CurrentSettings);
}
