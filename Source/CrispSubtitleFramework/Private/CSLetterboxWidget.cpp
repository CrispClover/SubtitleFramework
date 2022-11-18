// Copyright Crisp Clover.

#include "CSLetterboxWidget.h"
#include "CSProjectSettingFunctions.h"
#include "CSLineWidget.h"
#include "Components/Border.h"
#include "Components/BorderSlot.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/InvalidationBox.h"

void UCSLetterboxWidget::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	if (!LineContainer || !Background || !LineClass)
		return;

	ConstructFromSubtitle(UCSProjectSettingFunctions::GetDefaultExampleSubtitle(), UCSProjectSettingFunctions::GetDesignSettings());
}

void UCSLetterboxWidget::ConstructFromSubtitle_Implementation(FCrispSubtitle const& subtitle, UCSUserSettings* settings)
{
	LineContainer->ClearChildren();

	FLayoutCacheData const& layout = settings->GetLayout();
	Cast<UBorderSlot>(Background->GetContentSlot())->SetPadding(layout.BoxPadding);
	Background->SetBrushColor(settings->LetterboxColour);

	const bool hasLabel = !subtitle.Label.IsEmpty();
	FCSLineStyle const& lineStyle = settings->GetLineStyle(subtitle.Speaker);

	if (hasLabel)//Create label
	{
		UCSLineWidget* lineW = CreateWidget<UCSLineWidget>(GetWorld(), LineClass);
		UVerticalBoxSlot* slot = LineContainer->AddChildToVerticalBox(lineW);
		slot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);
		lineW->ConstructLine(subtitle.Label, lineStyle);
	}

	for (int32 i = 0; i < subtitle.Lines.Num(); i++)//Create lines
	{
		UCSLineWidget* lineW = CreateWidget<UCSLineWidget>(GetWorld(), LineClass);
		UVerticalBoxSlot* slot = LineContainer->AddChildToVerticalBox(lineW);
		slot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);
		lineW->ConstructLine(subtitle.Lines[i], lineStyle);

		if (hasLabel || i > 0)
			slot->SetPadding(layout.LinePadding);
	}
}
