// Copyright Crisp Clover.

#include "CSLetterboxWidget.h"
#include "CSProjectSettingFunctions.h"
#include "CSLineWidget.h"
#include "CSIndicatorWidget.h"
#include "Components/Border.h"
#include "Components/BorderSlot.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/InvalidationBox.h"

#if WITH_EDITOR
void UCSLetterboxWidget::eConstructExample(FVector2D const& size)
{
	Super::eConstructExample(size);

	if (!Background || !LineContainer || GetParent())
		return;

	TArray<FCrispSubtitle> const& subtitles = UCSProjectSettingFunctions::GetDefaultExampleSubtitle();

	if (subtitles.IsEmpty())
		return;

	FCSLetterboxStyle const& style = UCSUILibrary::GetDesignLetterboxStyle(subtitles[0].Speaker, subtitles[0].IsIndirectSpeech(), size);
	ConstructFromSubtitle(subtitles[0], style);
}
#endif

void UCSLetterboxWidget::ConstructFromSubtitle_Implementation(FCrispSubtitle const& subtitle, FCSLetterboxStyle const& style)
{
	LineContainer->ClearChildren();

	Cast<UBorderSlot>(Background->GetContentSlot())->SetPadding(style.BoxPadding);
	Background->SetBrushColor(style.LetterboxColour);

	const bool hasLabel = !subtitle.Label.IsEmpty();

	if (!style.LineClass)
		return;

	if (hasLabel)//Create label
	{
		UCSLineWidget* lineW = CreateWidget<UCSLineWidget>(this, style.LineClass);
		UVerticalBoxSlot* slot = LineContainer->AddChildToVerticalBox(lineW);
		slot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);
		lineW->ConstructLine(subtitle.Label, style.LineStyle);
	}

	for (int32 i = 0; i < subtitle.Lines.Num(); i++)//Create lines
	{
		UCSLineWidget* lineW = CreateWidget<UCSLineWidget>(this, style.LineClass);
		UVerticalBoxSlot* slot = LineContainer->AddChildToVerticalBox(lineW);
		slot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);
		lineW->ConstructLine(subtitle.Lines[i], style.LineStyle);

		if (hasLabel || i > 0)
			slot->SetPadding(style.LinePadding);
	}
	
	if (!Indicator)
		return;

	if (style.bShowIndicator)
		Indicator->Register(FCSSoundID(subtitle.Source, UCSProjectSettingFunctions::GetSoundNameForSpeech()));
	else
		Indicator->RemoveFromParent();
}
