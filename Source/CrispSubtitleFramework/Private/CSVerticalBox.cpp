// Copyright Crisp Clover.

#include "CSVerticalBox.h"
#include "CSVerticalBoxSlot.h"
#include "CSBaseSpacer.h"
#include "CSUILibrary.h"

UCSVerticalBoxSlot* UCSVerticalBox::rFindSlot(const int32 id)
{
	const int32 x = iIDs.Find(id);

	if (x == INDEX_NONE)
		return nullptr;
	else
		return Cast<UCSVerticalBoxSlot>(Slots[x]);
}

UWidget* UCSVerticalBox::rFindChild(const int32 id)
{
	const int32 x = iIDs.Find(id);

	if (x == INDEX_NONE)
		return nullptr;
	else
		return Slots[x]->Content;
}

UCSVerticalBoxSlot* UCSVerticalBox::FindOrAddSlot(UWidget* newChild, const float tNow, const float dtGap, const int32 id)
{
    const int32 c = iIDs.Num();
	for (int32 x = 0; x < c; x++)
	{
		if (iVacant[x] && iFlickerData[x].IsAvailable(tNow, dtGap))
		{
            uSetData(x, CSFlickerData(tNow), false, id);
			uReplaceChildAt(x, newChild);
			return Cast<UCSVerticalBoxSlot>(Slots[x]);
		}
	}

    iAddData(CSFlickerData(tNow), false, id);
	UCSVerticalBoxSlot* slot = Cast<UCSVerticalBoxSlot>(AddChild(newChild));
	return slot;
}

UCSVerticalBoxSlot* UCSVerticalBox::AddVacantSlot(FCSSpacerInfo const& info, const float tNow)
{
    if (!info.SpacerClass)
        return nullptr;

	UCSBaseSpacer* spacer = CreateWidget<UCSBaseSpacer>(this, info.SpacerClass);
	spacer->SetSize(info.Size);//TODO: calculate based on settings?

    iAddData(CSFlickerData(tNow), true);
	UCSVerticalBoxSlot* slot = Cast<UCSVerticalBoxSlot>(AddChild(spacer));
	return slot;
}

float UCSVerticalBox::dtTryVacate(const int32 id, FCSSpacerInfo const& info, const float tNow, const float dtGap)
{
    if (!info.SpacerClass)
        return -1.f;

	UCSBaseSpacer* spacer = CreateWidget<UCSBaseSpacer>(this, info.SpacerClass);
	//spacer->SetSize(info.Size); //TODO: calculate based on settings?
    
    const int32 c = iIDs.Num();
    for (int32 x = 0; x < c; x++)
        if (iIDs[x] == id)
            return udtTryVacate(x, spacer, tNow, dtGap);
    
    return -1.f;
}

void UCSVerticalBox::ClearVacant(const float tNow, const float dtGap)
{
	for (int32 x = iVacant.Num() - 1; x >= 0; x--)
	{
		if (iVacant[x] && iFlickerData[x].IsAvailable(tNow, dtGap))
		{
			uRemoveData(x);
			RemoveChildAt(x);
		}
	}
}

void UCSVerticalBox::ClearExcessVacant(const float tNow, const float dtGap)
{
	for (int32 x = iVacant.Num() - 1; x >= 0; x--)
	{
		if (!iVacant[x])
			return;
		
		uRemoveData(x);
		RemoveChildAt(x);
	}
}

UClass* UCSVerticalBox::GetSlotClass() const
{
	return UCSVerticalBoxSlot::StaticClass();
}

void UCSVerticalBox::OnSlotAdded(UPanelSlot* slot)
{
	if (iVerticalBox.IsValid())
		CastChecked<UCSVerticalBoxSlot>(slot)->AddSlot(iVerticalBox.ToSharedRef());
}

void UCSVerticalBox::OnSlotRemoved(UPanelSlot* slot)
{
	if (iVerticalBox.IsValid() && slot->Content)
	{
		TSharedPtr<SWidget> widget = slot->Content->GetCachedWidget();

		if (widget.IsValid())
			iVerticalBox->RemoveSlot(widget.ToSharedRef());
	}
}

void UCSVerticalBox::ReleaseSlateResources(bool releaseChildren)
{
	Super::ReleaseSlateResources(releaseChildren);

	iVerticalBox.Reset();
}

TSharedRef<SWidget> UCSVerticalBox::RebuildWidget()
{
	iVerticalBox = SNew(SVerticalBox);

	for (UPanelSlot* panelSlot : Slots)
	{
		if (UCSVerticalBoxSlot* slot = Cast<UCSVerticalBoxSlot>(panelSlot))
		{
			slot->Parent = this;
			slot->AddSlot(iVerticalBox.ToSharedRef());
		}
	}

	return iVerticalBox.ToSharedRef();
}

float UCSVerticalBox::udtTryVacate(int32 x, UWidget* spacer, const float tNow, const float dtGap)
{
	const float dtMissing = iFlickerData[x].dtMissing(tNow, dtGap);

	if (dtMissing <= 0.f)
	{
		uSetData(x, CSFlickerData(tNow), true);
		uReplaceChildAt(x, spacer);
	}

	return dtMissing;
}

void UCSVerticalBox::uReplaceChildAt(const int32 x, UWidget* newChild)
{
	UPanelSlot* slot = Slots[x];

	if (UWidget* oldChild = slot->Content)
	{
		oldChild->Slot = nullptr;
		
		if (UCSBaseSpacer* spacer = Cast<UCSBaseSpacer>(newChild))
			spacer->SetSize(oldChild->GetDesiredSize());//TODO: calculate based on settings?
	}

	Slots.RemoveAt(x);
	OnSlotRemoved(slot);

	slot->ReleaseSlateResources(true);
	slot->Parent = nullptr;
	slot->Content = nullptr;

	if (newChild)
	{
		newChild->RemoveFromParent();
		newChild->Slot = slot;
	}

	slot->Content = newChild;
	Slots.EmplaceAt(x, slot);

	if (iVerticalBox.IsValid())
		CastChecked<UCSVerticalBoxSlot>(slot)->InsertSlot(iVerticalBox.ToSharedRef(), x);

	InvalidateLayoutAndVolatility();
}

void UCSVerticalBox::iAddData(CSFlickerData const& flickerData, const bool vacant, const int32 id)
{
    iFlickerData.Add(flickerData);
    iIDs.Add(id);
    iVacant.Add(vacant);
}

void UCSVerticalBox::uSetData(const int32 x, CSFlickerData const& flickerData, const bool vacant, const int32 id)
{
    iFlickerData[x] = flickerData;
    iIDs[x] = id;
    iVacant[x] = vacant;
}

void UCSVerticalBox::uRemoveData(const int32 x)
{
    iFlickerData.RemoveAt(x);
    iIDs.RemoveAt(x);
    iVacant.RemoveAt(x);
}