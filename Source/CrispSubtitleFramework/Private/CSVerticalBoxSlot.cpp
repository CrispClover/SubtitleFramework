// Copyright Crisp Clover.

#include "CSVerticalBoxSlot.h"
#include "Components/Widget.h"
#include "CSVerticalBox.h"

#if WITH_EDITOR
bool UCSVerticalBoxSlot::NudgeByDesigner(FVector2D const& dNudge, TOptional<int32> const& gridSnapSize)
{
	if (dNudge.Y == 0)
		return false;
	
	const FVector2D dClampedNudge = dNudge.ClampAxes(-1, 1);
	UCSVerticalBox* parent = CastChecked<UCSVerticalBox>(Parent);

	const int32 x = parent->GetChildIndex(Content);

	if ((x == 0 && dClampedNudge.Y < 0.0f) || (x + 1 >= parent->GetChildrenCount() && dClampedNudge.Y > 0.0f))
		return false;

	parent->Modify();
	parent->ShiftChild(x + dClampedNudge.Y, Content);

	return true;
}

void UCSVerticalBoxSlot::SynchronizeFromTemplate(UPanelSlot const* const templateSlot)
{
	ThisClass const* const slot = CastChecked<ThisClass>(templateSlot);
	const int32 x = slot->Parent->GetChildIndex(slot->Content);

	UCSVerticalBox* parent = CastChecked<UCSVerticalBox>(Parent);
	parent->ShiftChild(x, Content);
}
#endif

UCSVerticalBoxSlot::UCSVerticalBoxSlot(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, iSlot(nullptr)
{
	HorizontalAlignment = HAlign_Fill;
	VerticalAlignment = VAlign_Fill;
	Size = FSlateChildSize(ESlateSizeRule::Automatic);
}

void UCSVerticalBoxSlot::ReleaseSlateResources(bool releaseChildren)
{
	Super::ReleaseSlateResources(releaseChildren);

	iSlot = nullptr;
}

void UCSVerticalBoxSlot::AddSlot(TSharedRef<SVerticalBox> verticalBox)
{
	verticalBox->AddSlot()
		.Expose(iSlot)
		.Padding(Padding)
		.HAlign(HorizontalAlignment)
		.VAlign(VerticalAlignment)
		.SizeParam(UWidget::ConvertSerializedSizeParamToRuntime(Size))
		.Expose(iSlot)
		[
			Content == nullptr ? SNullWidget::NullWidget : Content->TakeWidget()
		];
}

void UCSVerticalBoxSlot::InsertSlot(TSharedRef<SVerticalBox> verticalBox, const int32 x)
{
	verticalBox->InsertSlot(x)
		.Expose(iSlot)
		.Padding(Padding)
		.HAlign(HorizontalAlignment)
		.VAlign(VerticalAlignment)
		.SizeParam(UWidget::ConvertSerializedSizeParamToRuntime(Size))
		.Expose(iSlot)
		[
			Content == nullptr ? SNullWidget::NullWidget : Content->TakeWidget()
		];
}

void UCSVerticalBoxSlot::SetPadding(FMargin padding)
{
	Padding = padding;

	if (iSlot)
		iSlot->SetPadding(padding);
}

void UCSVerticalBoxSlot::SetSize(FSlateChildSize size)
{
	Size = size;
	
	if (iSlot)
		iSlot->SetSizeParam(UWidget::ConvertSerializedSizeParamToRuntime(size));
}

void UCSVerticalBoxSlot::SetHorizontalAlignment(EHorizontalAlignment horizontalAlignment)
{
	HorizontalAlignment = horizontalAlignment;

	if (iSlot)
		iSlot->SetHorizontalAlignment(horizontalAlignment);
}

void UCSVerticalBoxSlot::SetVerticalAlignment(EVerticalAlignment verticalAlignment)
{
	VerticalAlignment = verticalAlignment;

	if (iSlot)
		iSlot->SetVerticalAlignment(verticalAlignment);
}

void UCSVerticalBoxSlot::SynchronizeProperties()
{
	SetPadding(Padding);
	SetSize(Size);
	SetHorizontalAlignment(HorizontalAlignment);
	SetVerticalAlignment(VerticalAlignment);
}