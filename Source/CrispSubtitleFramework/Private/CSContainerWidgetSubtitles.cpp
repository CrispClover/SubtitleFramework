// Copyright Crisp Clover.

#include "CSContainerWidgetSubtitles.h"
#include "CSS_SubtitleGISS.h"
#include "CSUserSettings.h"
#include "CSLetterboxWidget.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"

void UCSContainerWidgetSubtitles::NativeConstruct()
{
	Super::NativeConstruct();

	oCSS->ConstructSubtitleEvent.AddDynamic(this, &UCSContainerWidgetSubtitles::OnSubtitleReceived);
	oCSS->DestroySubtitleEvent.AddDynamic(this, &UCSContainerWidgetSubtitles::OnDestroy);
	oCSS->ReconstructSubtitlesEvent.AddDynamic(this, &UCSContainerWidgetSubtitles::OnReconstruct);
	oCSS->RecalculateLayout();
}

void UCSContainerWidgetSubtitles::NativeDestruct()
{
	oCSS->ConstructSubtitleEvent.RemoveAll(this);
	oCSS->DestroySubtitleEvent.RemoveAll(this);
	oCSS->ReconstructSubtitlesEvent.RemoveAll(this);

	Super::NativeDestruct();
}

UVerticalBoxSlot* UCSContainerWidgetSubtitles::GetSlot(const int32 id)
{
	const int32 index = iChildrenData.rxFind(id);

	if (index == INDEX_NONE)
		return nullptr;
	else
		return iChildrenData.Slots[index];
}

UCSLetterboxWidget* UCSContainerWidgetSubtitles::GetLetterbox(const int32 id)
{
	const int32 index = iChildrenData.rxFind(id);

	if (index == INDEX_NONE)
		return nullptr;
	else
		return iChildrenData.Children[index];
}

void UCSContainerWidgetSubtitles::OnSubtitleReceived_Implementation(FCrispSubtitle const& subtitle)
{
	UCSUserSettings* settings = oCSS->GetCurrentSettings();

	UCSLetterboxWidget* letterbox = CreateWidget<UCSLetterboxWidget>(GetWorld(), settings->LetterboxClass.LoadSynchronous());
	UVerticalBoxSlot* slot = Container->AddChildToVerticalBox(letterbox);
	slot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);
	slot->SetPadding(settings->GetLayout().SubtitlePadding);
	letterbox->ConstructFromSubtitle(subtitle, settings);
	iChildrenData.Add(subtitle.ID, letterbox, slot);
}

void UCSContainerWidgetSubtitles::OnDestroy_Implementation(const int32 id)
{
	if (UCSLetterboxWidget* letterbox = iChildrenData.rConsume(id))
		letterbox->RemoveFromParent();
}

void UCSContainerWidgetSubtitles::OnReconstruct_Implementation(TArray<FCrispSubtitle> const& subtitles)
{
	UCSUserSettings* settings = oCSS->GetCurrentSettings();

	const int32 cSubtitles = subtitles.Num();
	const int32 cWidgets = iChildrenData.Num();
	const int32 dc = cWidgets - cSubtitles;

	for (int32 i = 1; i <= dc; i++)//Remove excess
		iChildrenData.Children[cWidgets - i]->RemoveFromParent();

	for (int32 i = 0; i < cWidgets; i++)//Reconstruct existing
		iChildrenData.Children[i]->ConstructFromSubtitle(subtitles[i], settings);

	for (UVerticalBoxSlot* slot : iChildrenData.Slots)
		slot->SetPadding(settings->SubtitlePadding);

	for (int32 i = dc; i < 0; i++)//Add missing
		OnSubtitleReceived(subtitles[cSubtitles - i]);
}