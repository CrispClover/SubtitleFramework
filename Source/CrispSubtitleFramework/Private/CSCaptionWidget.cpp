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
	FCSCaptionStyle const& style = UCSUILibrary::GetDesignCaptionStyle(caption.SoundID.Source);
	ConstructFromCaption(caption, style);
#endif
}

void UCSCaptionWidget::ConstructFromCaption_Implementation(FCrispCaption const& caption, FCSCaptionStyle const& style)
{
	Text->SetText(caption.Description);
	Text->SetColorAndOpacity(style.TextColour);
	Text->SetFont(style.FontInfo);

	Background->SetBrushColor(style.BackColour);

	if (!Indicator)
		return;

	if (style.bShowIndicator)
		Indicator->Register(caption.SoundID);
	else
		Indicator->RemoveFromParent();
}
