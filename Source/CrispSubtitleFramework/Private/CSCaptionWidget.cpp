// Copyright Crisp Clover.

#include "CSCaptionWidget.h"
#include "CSProjectSettingFunctions.h"
#include "CSUserSettings.h"
#include "CSIndicatorWidget.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"
#include "Components/InvalidationBox.h"

#if WITH_EDITOR
void UCSCaptionWidget::eConstructExample(FVector2D const& size)
{
	Super::eConstructExample(size);

	if (!Background || !Text || GetParent())
		return;
		
	TArray<FCrispCaption> const& captions = UCSProjectSettingFunctions::GetExampleCaptions();

	if (captions.IsEmpty())
		return;

	FCSCaptionStyle const& style = UCSUILibrary::GetDesignCaptionStyle(captions[0].SoundID.Source, size);
	ConstructFromCaption(captions[0], style);
}
#endif

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
