// Copyright Crisp Clover.

#include "CSContainerWidgetSubtitles.h"
#include "CSS_SubtitleGISS.h"
#include "CSUserSettings.h"
#include "CSUILibrary.h"
#include "CSLetterboxWidget.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"

void UCSContainerWidgetSubtitles::eConstructExample(FVector2D const& size)
{
	Super::eConstructExample(size);

	if (!Container)
		return;

	UCSUserSettings* settings = UCSProjectSettingFunctions::GetDesignSettings(FVector2D());
	FCrispSubtitle const& subtitle = UCSProjectSettingFunctions::GetExampleSubtitle(settings);
	FCSLetterboxStyle const& style = UCSUILibrary::GetDesignLetterboxStyle(subtitle.Speaker, size);//TODO: mismatches

	if (!euExample)
		euExample = CreateWidget<UCSLetterboxWidget>(this, settings->LetterboxClass.LoadSynchronous());
	
	Container->RemoveChild(euExample);
	UVerticalBoxSlot* slot = Container->AddChildToVerticalBox(euExample);

	slot->SetPadding(settings->GetLayout().SubtitlePadding);
	euExample->ConstructFromSubtitle(subtitle, style);
}

void UCSContainerWidgetSubtitles::NativeConstruct()
{
	Super::NativeConstruct();

	uCSS->ConstructSubtitleEvent.AddDynamic(this, &UCSContainerWidgetSubtitles::OnSubtitleReceived);
	uCSS->DestroySubtitleEvent.AddDynamic(this, &UCSContainerWidgetSubtitles::OnDestroy);
	uCSS->ReconstructSubtitlesEvent.AddDynamic(this, &UCSContainerWidgetSubtitles::OnReconstruct);
	uCSS->RecalculateLayout();
}

void UCSContainerWidgetSubtitles::NativeDestruct()
{
	uCSS->ConstructSubtitleEvent.RemoveAll(this);
	uCSS->DestroySubtitleEvent.RemoveAll(this);
	uCSS->ReconstructSubtitlesEvent.RemoveAll(this);

	Super::NativeDestruct();
}

UVerticalBoxSlot* UCSContainerWidgetSubtitles::GetSlot(const int32 id)
{
	const int32 ux = iChildrenData.rxFind(id);

	if (ux == INDEX_NONE)
		return nullptr;
	else
		return iChildrenData.Slots[ux];
}

UCSLetterboxWidget* UCSContainerWidgetSubtitles::GetLetterbox(const int32 id)
{
	const int32 ux = iChildrenData.rxFind(id);

	if (ux == INDEX_NONE)
		return nullptr;
	else
		return iChildrenData.Children[ux];
}

void UCSContainerWidgetSubtitles::OnSubtitleReceived_Implementation(FCrispSubtitle const& subtitle)
{
	UCSUserSettings* settings = uCSS->GetCurrentSettings();

	UCSLetterboxWidget* letterbox = CreateWidget<UCSLetterboxWidget>(this, settings->LetterboxClass.LoadSynchronous());
	UVerticalBoxSlot* slot = Container->AddChildToVerticalBox(letterbox);

	if (!slot)
		return;

	slot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);
	slot->SetPadding(settings->GetLayout().SubtitlePadding);
	letterbox->ConstructFromSubtitle(subtitle, UCSUILibrary::GetLetterboxStyle(settings, subtitle.Speaker, subtitle.IsIndirectSpeech()));
	iChildrenData.Add(subtitle.ID, letterbox, slot);
}

void UCSContainerWidgetSubtitles::OnDestroy_Implementation(const int32 id)
{
	if (UCSLetterboxWidget* letterbox = iChildrenData.rConsume(id))
		letterbox->RemoveFromParent();
}

void UCSContainerWidgetSubtitles::OnReconstruct_Implementation(TArray<FCrispSubtitle> const& subtitles)
{
	UCSUserSettings* settings = uCSS->GetCurrentSettings();

	const int32 cSubtitles = subtitles.Num();
	const int32 cWidgets = iChildrenData.Num();
	const int32 dc = cWidgets - cSubtitles;

	for (int32 i = 1; i <= dc; i++)//Remove excess
		iChildrenData.Children[cWidgets - i]->RemoveFromParent();

	for (int32 i = 0; i < cSubtitles; i++)//Reconstruct existing
		iChildrenData.Children[i]->ConstructFromSubtitle(subtitles[i], UCSUILibrary::GetLetterboxStyle(settings, subtitles[i].Speaker, subtitles[i].IsIndirectSpeech()));

	for (UVerticalBoxSlot* slot : iChildrenData.Slots)
		slot->SetPadding(settings->SubtitlePadding);

	for (int32 ni = dc; ni < 0; ni++)//Add missing
		OnSubtitleReceived(subtitles[cSubtitles + ni]);
}