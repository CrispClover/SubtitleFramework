// Copyright Crisp Clover.

#include "CSLineWidget.h"
#include "CSUserSettings.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "Components/BorderSlot.h"

void UCSLineWidget::ConstructLine_Implementation(FText const& content, FCSLineStyle const& lineStyle)
{
	Text->SetText(content);
	Text->SetFont(lineStyle.FontInfo);
	Text->SetColorAndOpacity(lineStyle.TextColour);

	Cast<UBorderSlot>(Background->GetContentSlot())->SetPadding(lineStyle.TextPadding);
	Background->SetBrushColor(lineStyle.LineBackColour);
}