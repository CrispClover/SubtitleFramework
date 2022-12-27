// Copyright Crisp Clover.

#include "CSVerticalBox.h"
#include "CSVerticalBoxSlot.h"
#include "CSBaseSpacer.h"

UCSVerticalBoxSlot* UCSVerticalBox::AddDesignChild(UWidget* child)
{
	if (!child)
		return nullptr;

	return Cast<UCSVerticalBoxSlot>(iAddChild(child));
}

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
	if (!newChild)
		return nullptr;

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

	return iAddChild(newChild, tNow, false, id);
}

UCSVerticalBoxSlot* UCSVerticalBox::AddVacantSlot(TSubclassOf<UCSBaseSpacer> spacerClass, FVector2D size, const float tNow)
{
    if (!spacerClass)
        return nullptr;

	UCSBaseSpacer* spacer = CreateWidget<UCSBaseSpacer>(this, spacerClass);
	spacer->SetSize(size);//TODO: calculate based on settings?
	
	return iAddChild(spacer, tNow, true);
}

float UCSVerticalBox::dtTryVacate(const int32 id, TSubclassOf<UCSBaseSpacer> spacerClass, const float tNow, const float dtGap)
{
    if (!spacerClass)
        return -1.f;

	UCSBaseSpacer* spacer = CreateWidget<UCSBaseSpacer>(this, spacerClass);
    
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

	Slots.RemoveAt(x);

	if (UWidget* oldChild = slot->Content)
	{
		oldChild->Slot = nullptr;

		if (UCSBaseSpacer* spacer = Cast<UCSBaseSpacer>(newChild))
			spacer->SetSize(oldChild->GetDesiredSize());

		TSharedPtr<SWidget> widget = oldChild->GetCachedWidget();

		if (iVerticalBox.IsValid() && widget.IsValid())
			iVerticalBox->RemoveSlot(widget.ToSharedRef());
	}

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

UPanelSlot* UCSVerticalBox::iAddChild(UWidget* child)
{
	if (!child)
		return nullptr;

	child->RemoveFromParent();

	EObjectFlags flags = RF_Transactional;
	if (HasAnyFlags(RF_Transient))
		flags |= RF_Transient;

	UPanelSlot* slot = NewObject<UPanelSlot>(this, GetSlotClass(), NAME_None, flags);
	slot->Content = child;
	slot->Parent = this;

	child->Slot = slot;

	if (bAddToTop)
	{
		Slots.Insert(slot, 0);
	}
	else
	{
		Slots.Add(slot);
	}

	OnSlotAdded(slot);

	InvalidateLayoutAndVolatility();

	return slot;
}

UCSVerticalBoxSlot* UCSVerticalBox::iAddChild(UWidget* child, const float tNow, const bool vacant, const int32 id)
{
	if (bAddToTop)
		uInsertData(0, CSFlickerData(tNow), vacant, id);
	else
		iAddData(CSFlickerData(tNow), vacant, id);

	return Cast<UCSVerticalBoxSlot>(AddChild(child));
}

void UCSVerticalBox::iAddData(CSFlickerData const& flickerData, const bool vacant, const int32 id)
{
    iFlickerData.Add(flickerData);
    iIDs.Add(id);
    iVacant.Add(vacant);
}

void UCSVerticalBox::uInsertData(const int32 x, CSFlickerData const& flickerData, const bool vacant, const int32 id)
{
	iFlickerData.Insert(flickerData, x);
    iIDs.Insert(id, x);
    iVacant.Insert(vacant, x);
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