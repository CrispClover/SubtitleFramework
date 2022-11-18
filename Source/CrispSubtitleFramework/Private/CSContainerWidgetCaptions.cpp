// Copyright Crisp Clover.

#include "CSContainerWidgetCaptions.h"
#include "CSS_SubtitleGISS.h"
#include "CSCaptionWidget.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"

void UCSContainerWidgetCaptions::NativeConstruct()
{
	Super::NativeConstruct();

	oCSS->ConstructCaptionEvent.AddDynamic(this, &UCSContainerWidgetCaptions::OnCaptionReceived);
	oCSS->DestructCaptionEvent.AddDynamic(this, &UCSContainerWidgetCaptions::OnDestroy);
	oCSS->ReconstructCaptionsEvent.AddDynamic(this, &UCSContainerWidgetCaptions::OnReconstruct);
}

void UCSContainerWidgetCaptions::NativeDestruct()
{
	oCSS->ConstructCaptionEvent.RemoveAll(this);
	oCSS->DestructCaptionEvent.RemoveAll(this);
	oCSS->ReconstructCaptionsEvent.RemoveAll(this);

	Super::NativeDestruct();
}

UVerticalBoxSlot* UCSContainerWidgetCaptions::GetSlot(const int32 id)
{
	const int32 index = iChildrenData.xFind(id);

	if (index == INDEX_NONE)
		return nullptr;
	else
		return iChildrenData.Slots[index];
}

UCSCaptionWidget* UCSContainerWidgetCaptions::GetCaptionWidget(const int32 id)
{
	const int32 index = iChildrenData.xFind(id);

	if (index == INDEX_NONE)
		return nullptr;
	else
		return iChildrenData.Children[index];
}

void UCSContainerWidgetCaptions::OnCaptionReceived_Implementation(FCrispCaption const& caption)
{
	UCSUserSettings* settings = oCSS->GetCurrentSettings();

	UCSCaptionWidget* captionWidget;
	if (settings->bShowIndicators)
		captionWidget = CreateWidget<UCSCaptionWidget>(GetWorld(), IndicatorCaptionClass);
	else
		captionWidget = CreateWidget<UCSCaptionWidget>(GetWorld(), CaptionClass);

	UVerticalBoxSlot* slot = Container->AddChildToVerticalBox(captionWidget);
	slot->SetPadding(settings->GetLayout().CaptionPadding);
	captionWidget->ConstructFromCaption(caption, settings);
	iChildrenData.Add(caption.ID, captionWidget, slot);
}

void UCSContainerWidgetCaptions::OnDestroy_Implementation(const int32 id)
{
	if (UCSCaptionWidget* captionWidget = iChildrenData.Consume(id))
		captionWidget->RemoveFromParent();
}

void UCSContainerWidgetCaptions::OnReconstruct_Implementation(TArray<FCrispCaption> const& captions)
{
	UCSUserSettings* settings = oCSS->GetCurrentSettings();

	const int32 cCaptions = captions.Num();
	const int32 cWidgets = iChildrenData.Num();
	const int32 dcCaptions = cWidgets - cCaptions;

	for (int32 i = 1; i <= dcCaptions; i++)//Remove excess
		iChildrenData.Children[cWidgets - i]->RemoveFromParent();

	for (int32 i = 0; i < cWidgets; i++)//Reconstruct existing
		iChildrenData.Children[i]->ConstructFromCaption(captions[i], settings);

	for (UVerticalBoxSlot* slot : iChildrenData.Slots)//Apply padding to existing
		slot->SetPadding(settings->CaptionPadding);

	for (int32 i = dcCaptions; i < 0; i++)//Add missing
		OnCaptionReceived(captions[cCaptions - i]);
}