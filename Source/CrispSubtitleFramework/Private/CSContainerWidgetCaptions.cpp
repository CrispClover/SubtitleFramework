// Copyright Crisp Clover.

#include "CSContainerWidgetCaptions.h"
#include "CSS_SubtitleGISS.h"
#include "CSUserSettings.h"
#include "CSCaptionWidget.h"
#include "CSUILibrary.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"

void UCSContainerWidgetCaptions::NativeConstruct()
{
	Super::NativeConstruct();

	uCSS->ConstructCaptionEvent.AddDynamic(this, &UCSContainerWidgetCaptions::OnCaptionReceived);
	uCSS->DestructCaptionEvent.AddDynamic(this, &UCSContainerWidgetCaptions::OnDestroy);
	uCSS->ReconstructCaptionsEvent.AddDynamic(this, &UCSContainerWidgetCaptions::OnReconstruct);
	uCSS->RecalculateLayout();
}

void UCSContainerWidgetCaptions::NativeDestruct()
{
	uCSS->ConstructCaptionEvent.RemoveAll(this);
	uCSS->DestructCaptionEvent.RemoveAll(this);
	uCSS->ReconstructCaptionsEvent.RemoveAll(this);

	Super::NativeDestruct();
}

UVerticalBoxSlot* UCSContainerWidgetCaptions::GetSlot(const int32 id)
{
	const int32 ux = iChildrenData.rxFind(id);

	if (ux == INDEX_NONE)
		return nullptr;
	else
		return iChildrenData.Slots[ux];
}

UCSCaptionWidget* UCSContainerWidgetCaptions::GetCaptionWidget(const int32 id)
{
	const int32 ux = iChildrenData.rxFind(id);

	if (ux == INDEX_NONE)
		return nullptr;
	else
		return iChildrenData.Children[ux];
}

void UCSContainerWidgetCaptions::OnCaptionReceived_Implementation(FCrispCaption const& caption)
{
	UCSUserSettings* settings = uCSS->GetCurrentSettings();

	const FCSCaptionStyle style = UCSUILibrary::GetCaptionStyle(settings, caption.SoundID.Source);

	UCSCaptionWidget* captionWidget = CreateWidget<UCSCaptionWidget>(this, settings->CaptionClass.LoadSynchronous());
	UVerticalBoxSlot* slot = Container->AddChildToVerticalBox(captionWidget);

	slot->SetPadding(settings->GetLayout().CaptionPadding);
	captionWidget->ConstructFromCaption(caption, style);
	iChildrenData.Add(caption.ID, captionWidget, slot);
}

void UCSContainerWidgetCaptions::OnDestroy_Implementation(const int32 id)
{
	if (UCSCaptionWidget* captionWidget = iChildrenData.rConsume(id))
		captionWidget->RemoveFromParent();
}

void UCSContainerWidgetCaptions::OnReconstruct_Implementation(TArray<FCrispCaption> const& captions)
{
	UCSUserSettings* settings = uCSS->GetCurrentSettings();

	const int32 cCaptions = captions.Num();
	const int32 cWidgets = iChildrenData.Num();
	const int32 dcCaptions = cWidgets - cCaptions;

	for (int32 i = 1; i <= dcCaptions; i++)//Remove excess
		iChildrenData.Children[cWidgets - i]->RemoveFromParent();

	for (int32 i = 0; i < cCaptions; i++)//Reconstruct existing
		iChildrenData.Children[i]->ConstructFromCaption(captions[i], UCSUILibrary::GetCaptionStyle(settings, captions[i].SoundID.Source));

	for (UVerticalBoxSlot* slot : iChildrenData.Slots)//Apply padding to existing
		slot->SetPadding(settings->CaptionPadding);

	for (int32 ni = dcCaptions; ni < 0; ni++)//Add missing
		OnCaptionReceived(captions[cCaptions + ni]);
}