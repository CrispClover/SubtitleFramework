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

void UCSLetterboxWidget::SynchronizeProperties()
{
	Super::SynchronizeProperties();
		
#if WITH_EDITOR
	if (IsDesignTime() && Background && LineContainer)
		ConstructFromSubtitle(UCSProjectSettingFunctions::GetDefaultExampleSubtitle(), UCSProjectSettingFunctions::GetDesignSettings());
#endif
}

void UCSLetterboxWidget::ConstructFromSubtitle_Implementation(FCrispSubtitle const& subtitle, UCSUserSettings* settings)
{
	LineContainer->ClearChildren();

	FLayoutCacheData const& layout = settings->GetLayout();
	Cast<UBorderSlot>(Background->GetContentSlot())->SetPadding(layout.BoxPadding);
	Background->SetBrushColor(settings->LetterboxColour);

	const bool hasLabel = !subtitle.Label.IsEmpty();
	FCSLineStyle const& lineStyle = settings->GetLineStyle(subtitle.Speaker);

	if (settings->LineClass.IsNull())
		return;

	TSubclassOf<UCSLineWidget> lineClass = settings->LineClass.LoadSynchronous();

	if (hasLabel)//Create label
	{
		UCSLineWidget* lineW = CreateWidget<UCSLineWidget>(GetWorld(), lineClass);
		UVerticalBoxSlot* slot = LineContainer->AddChildToVerticalBox(lineW);
		slot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);
		lineW->ConstructLine(subtitle.Label, lineStyle);
	}

	for (int32 i = 0; i < subtitle.Lines.Num(); i++)//Create lines
	{
		UCSLineWidget* lineW = CreateWidget<UCSLineWidget>(GetWorld(), lineClass);
		UVerticalBoxSlot* slot = LineContainer->AddChildToVerticalBox(lineW);
		slot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);
		lineW->ConstructLine(subtitle.Lines[i], lineStyle);

		if (hasLabel || i > 0)
			slot->SetPadding(layout.LinePadding);
	}

	if (Indicator)
		Indicator->Register(FCSSoundID(subtitle.Source, UCSProjectSettingFunctions::GetSoundNameForSpeech()));
}
