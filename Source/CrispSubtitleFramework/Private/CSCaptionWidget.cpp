// Copyright Crisp Clover.

#include "CSCaptionWidget.h"
#include "CSProjectSettingFunctions.h"
#include "CSUserSettings.h"
#include "CSIndicatorWidget.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"

void UCSCaptionWidget::SynchronizeProperties()
{
	Super::SynchronizeProperties();
	
#if WITH_EDITOR
	if (!IsDesignTime() || !Background || !Text)
		return;

	FCrispCaption const& caption = UCSProjectSettingFunctions::GetExampleCaption();
	UCSUserSettings const* settings = UCSProjectSettingFunctions::GetDesignSettings();

	Text->SetText(caption.Description);
	Text->SetColorAndOpacity(settings->DefaultTextColour);

	Background->SetBrushColor(settings->CaptionBackColour);
#endif
}

void UCSCaptionWidget::ConstructFromCaption_Implementation(FCrispCaption const& caption, UCSUserSettings* settings)//TODO: switch to struct? use colour coding?
{
	Text->SetText(caption.Description);
	Text->SetColorAndOpacity(settings->DefaultTextColour);
	Text->SetFont(settings->GetLayout().FontInfo);//TODO: change size

	Background->SetBrushColor(settings->CaptionBackColour);

	if (!Indicator)
		return;

	if (settings->bShowCaptionIndicators)
		Indicator->Register(caption.SoundID);
	else
		Indicator->RemoveFromParent();
}
