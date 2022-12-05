// Copyright Crisp Clover.

#include "CSTrackingManager.h"
#include "Slate/SGameLayerManager.h"

void CSTrackingManager::Calculate()
{
	if (!Player || !Player->ViewportClient)
		return;

	FSceneViewProjectionData projectionData;
	if (!Player->GetProjectionData(Player->ViewportClient->Viewport, projectionData))
		return;

	const FMatrix projectionMatrix = projectionData.ComputeViewProjectionMatrix();
	FIntRect const& rectangle = projectionData.GetConstrainedViewRect();

	for (int32 i = 0; iData3D.NeedsCalc(i); i++)
	{
		FCSTrackedSoundData& sound3D = iData3D.AccessItem(i);

		const FVector4 projection = projectionMatrix.TransformFVector4(FVector4(sound3D.SoundData, 1.f));
		const float m = 1 - 2 * (int8)(projection.W < 0);//We want to mirror coordinates behind the player's view.

		//Normalised coordinates (X/W), offset subtracted, and mirrored in back:
		const FVector2f adjustedNDC = m * FVector2f(projection.X / projection.W - sound3D.Offset.X, -projection.Y / projection.W - sound3D.Offset.Y);

		sound3D.Angle = FMath::Atan2(adjustedNDC.Y * rectangle.Height(), adjustedNDC.X * rectangle.Width());
		sound3D.OpacityDriver = m * adjustedNDC.Length();//Negative numbers were unused. "Packing" projection.W < 0.
	}

	iDelegates3D.UpdateIDataEvent.Broadcast();
}

void CSTrackingManager::TrackSound(FCSSoundID const& id, FVector2D const& positionNDC)
{
	if (FCSIndicatorWidgetData* uData = iData2D.Find(id))
		uCalculateData2D(uData, positionNDC);
	else
		iLocations2D.FindOrAdd(FCSSoundID(id), positionNDC);
}

bool CSTrackingManager::GetSoundData(FCSSoundID const& id, FVector& data) const
{
	FCSTrackedSoundData const* ptr = iData3D.rFind(id);

	if (!ptr)
		return false;

	data = ptr->SoundData;
	return true;
}

CSIndicatorDelegates* CSTrackingManager::rRegisterIndicator(FCSRegisterArgs args)
{
	if (iData3D.Register(args.ID, args.WidgetDataPtrRef))
		return &iDelegates3D;
	else if (iRegister2D(args.ID, args.WidgetDataPtrRef))
		return &iDelegates2D;
	else
		return nullptr;
}

void CSTrackingManager::UnregisterIndicator(FCSSoundID const& id, UObject* widget)
{
	if (iData2D.Remove(id))
	{
		iDelegates2D.Remove(widget);
	}
	else
	{
		iDelegates3D.Remove(widget);

		FCSSwapArgs args = FCSSwapArgs(id, nullptr);

		iData3D.Unregister(id, args);

		if (args.WidgetDataPtr)
			iDelegates3D.SwapIDataEvent.Broadcast(args);
	}
}

void CSTrackingManager::Copy(CSTrackingManager const* manager)
{
	for (int32 i = 0; i < manager->iData3D.Num(); i++)
		TrackSound(manager->iData3D.GetID(i), manager->iData3D.GetItem(i).SoundData);
}

bool CSTrackingManager::iRegister2D(FCSSoundID id, FCSIndicatorWidgetData*& dataPtr)
{
	FVector2D pos;
	if (!iLocations2D.RemoveAndCopyValue(id, pos))
		return false;

	dataPtr = &iData2D.Add(id);
	uCalculateData2D(dataPtr, pos);
	return true;
}

void CSTrackingManager::uCalculateData2D(FCSIndicatorWidgetData* indicatorData, FVector2D const& positionNDC)
{
	if (!Player || !Player->ViewportClient)
		return;

	FSceneViewProjectionData projectionData;
	if (!Player->GetProjectionData(Player->ViewportClient->Viewport, projectionData))
		return;

	FIntRect const& rectangle = projectionData.GetConstrainedViewRect();

	const FVector2f offsetNDC = FVector2f(positionNDC.X - indicatorData->Offset.X, -positionNDC.Y - indicatorData->Offset.Y);

	indicatorData->Angle = FMath::Atan2(offsetNDC.Y * rectangle.Height(), offsetNDC.X * rectangle.Width());
	indicatorData->OpacityDriver = offsetNDC.Length();

	iDelegates2D.UpdateIDataEvent.Broadcast();
}
