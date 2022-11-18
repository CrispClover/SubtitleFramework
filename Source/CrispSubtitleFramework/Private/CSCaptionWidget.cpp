// Copyright Crisp Clover.

#include "CSCaptionWidget.h"
#include "CSProjectSettingFunctions.h"
#include "CSIndicatorWidget.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"

void UCSCaptionWidget::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	if (!Background || !Text)
		return;

	ConstructFromCaption(UCSProjectSettingFunctions::GetExampleCaption(), UCSProjectSettingFunctions::GetDesignSettings());
}

void UCSCaptionWidget::ConstructFromCaption_Implementation(FCrispCaption const& caption, UCSUserSettings* settings)
{
	Text->SetText(caption.Description);
	Text->SetColorAndOpacity(settings->DefaultTextColour);

	Background->SetBrushColor(settings->CaptionBackColour);

	if (!Indicator)
		return;
	
	Indicator->Register(caption.SoundID);
}
