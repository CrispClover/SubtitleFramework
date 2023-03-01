// Copyright Crisp Clover.

#include "CSTrackingManager.h"
#include "Slate/SGameLayerManager.h"

void CSTrackingManager::Calculate()
{
	if (!Player || !Player->ViewportClient)
		return;

	FSceneViewProjectionData projectionData;
	if (!Player->GetProjectionData(Player->ViewportClient->Viewport, EStereoscopicPass::eSSP_FULL, projectionData))
		return;

	const FMatrix projectionMatrix = projectionData.ComputeViewProjectionMatrix();
	FIntRect const& rectangle = projectionData.GetConstrainedViewRect();

	for (int32 i = 0; iData.NeedsCalc(i); i++)
	{
		CSTrackedSoundData& sound3D = iData.AccessItem(i);

		const FVector4 projection = projectionMatrix.TransformFVector4(FVector4(sound3D.SoundData, 1.f));
		const float m = 1 - 2 * (int8)(projection.W < 0);//We want to mirror coordinates behind the player's view.

		//Normalised coordinates (X/W), offset subtracted, and mirrored in back:
		const FVector2D adjustedNDC = m * FVector2D(projection.X / projection.W - sound3D.Offset.X, -projection.Y / projection.W - sound3D.Offset.Y);

		sound3D.Angle = FMath::Atan2(adjustedNDC.Y * rectangle.Height(), adjustedNDC.X * rectangle.Width());
		sound3D.OpacityDriver = m * adjustedNDC.Size();//Negative numbers were unused. "Packing" projection.W < 0.
	}

	UpdateIDataEvent.Broadcast();
}

void CSTrackingManager::TrackSound(FCSSoundID const& id, FVector const& data)
{
	if (iData.TrackSound(id, data))
		iNotifyOfNewTrackedSound(id);
}

bool CSTrackingManager::GetSoundData(FCSSoundID const& id, FVector& data) const
{
	CSTrackedSoundData const* ptr = iData.rFind(id);

	if (!ptr)
		return false;

	data = ptr->SoundData;
	return true;
}

void CSTrackingManager::UnregisterIndicator(FCSSoundID const& id, UObject* widget)
{
	FDelegateHandle handle;
	if (iPendingIndicatorDelegates.RemoveAndCopyValue(id, handle))
	{
		iNewSoundTrackedEvent.Remove(handle);
	}
	else
	{
		UpdateIDataEvent.RemoveAll(widget);
		iSwapDataEvent.RemoveAll(widget);

		CSSwapArgs args = CSSwapArgs(id, nullptr);

		iData.Unregister(id, args);

		if (args.WidgetDataPtr)
			iSwapDataEvent.Broadcast(args);
	}
}

void CSTrackingManager::Copy(CSTrackingManager const* manager)
{
	for (int32 i = 0; i < manager->iData.Num(); i++)
		TrackSound(manager->iData.GetID(i), manager->iData.GetItem(i).SoundData);
}

void CSTrackingManager::iNotifyOfNewTrackedSound(FCSSoundID const& id)
{
	FDelegateHandle handle;
	if (iPendingIndicatorDelegates.RemoveAndCopyValue(id, handle))
	{
		iNewSoundTrackedEvent.Broadcast(id);
		iNewSoundTrackedEvent.Remove(handle);
	}
}
