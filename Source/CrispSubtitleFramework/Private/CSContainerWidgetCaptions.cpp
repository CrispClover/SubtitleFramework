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

	oCSS->ConstructCaptionEvent.AddDynamic(this, &UCSContainerWidgetCaptions::OnCaptionReceived);
	oCSS->DestructCaptionEvent.AddDynamic(this, &UCSContainerWidgetCaptions::OnDestroy);
	oCSS->ReconstructCaptionsEvent.AddDynamic(this, &UCSContainerWidgetCaptions::OnReconstruct);
	oCSS->RecalculateLayout();
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
	UCSUserSettings* settings = oCSS->GetCurrentSettings();

	const FCSCaptionStyle style = UCSUILibrary::GetCaptionStyle(settings, caption.SoundID.Source);

	UCSCaptionWidget* captionWidget = CreateWidget<UCSCaptionWidget>(GetWorld(), settings->CaptionClass.LoadSynchronous());
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
	UCSUserSettings* settings = oCSS->GetCurrentSettings();

	const int32 cCaptions = captions.Num();
	const int32 cWidgets = iChildrenData.Num();
	const int32 dcCaptions = cWidgets - cCaptions;

	for (int32 i = 1; i <= dcCaptions; i++)//Remove excess
		iChildrenData.Children[cWidgets - i]->RemoveFromParent();

	for (int32 i = 0; i < cWidgets; i++)//Reconstruct existing
		iChildrenData.Children[i]->ConstructFromCaption(captions[i], UCSUILibrary::GetCaptionStyle(settings, captions[i].SoundID.Source));

	for (UVerticalBoxSlot* slot : iChildrenData.Slots)//Apply padding to existing
		slot->SetPadding(settings->CaptionPadding);

	for (int32 i = dcCaptions; i < 0; i++)//Add missing
		OnCaptionReceived(captions[cCaptions - i]);
}