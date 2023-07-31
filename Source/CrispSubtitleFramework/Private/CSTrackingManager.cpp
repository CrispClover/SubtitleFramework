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

	for (int32 i = 0; iData.NeedsCalc(i); i++)
	{
		CSTrackedSoundData& sound3D = iData.vAccessItem(i);

		const FVector4 projection = projectionMatrix.TransformFVector4(FVector4(sound3D.pSound, 1.f));
		const float m = 1 - 2 * (int8)(projection.W < 0);//We want to mirror coordinates behind the player's view.

		
		const FVector2f adjustedNDC //= Normalised coordinates (X/W), offset subtracted, and mirrored in back
			= m * FVector2f(projection.X / projection.W - sound3D.Offset.X, -projection.Y / projection.W - sound3D.Offset.Y);

		sound3D.Angle = FMath::Atan2(adjustedNDC.Y * rectangle.Height(), adjustedNDC.X * rectangle.Width());
		sound3D.OpacityDriver = m * adjustedNDC.Length();//Negative numbers were unused. "Packing" projection.W < 0.
	}

	UpdateIDataEvent.Broadcast();
}

void CSTrackingManager::TrackSound(FCSSoundID const& id, FVector const& pSound)
{
	if (iData.TrackSound(id, pSound))
	{
		iNotifyOfNewTrackedSound(id);
	}
}

bool CSTrackingManager::GetSoundData(FCSSoundID const& id, FVector& pSound) const
{
	CSTrackedSoundData const* trackedData = iData.uFind(id);

	if (!trackedData)
		return false;

	pSound = trackedData->pSound;
	return true;
}

void CSTrackingManager::UnregisterIndicator(FCSSoundID const& id, UObject* widget)
{
	FDelegateHandle handle;
	if (iPendingIndicatorDelegates.RemoveAndCopyValue(id, handle))
	{
		NewSoundTrackedEvent.Remove(handle);
	}
	else
	{
		UpdateIDataEvent.RemoveAll(widget);
		SwapDataEvent.RemoveAll(widget);

		CSSwapArgs args = CSSwapArgs(id, nullptr);

		iData.Unregister(id, args);

		if (args.WidgetDataPtr)
		{
			SwapDataEvent.Broadcast(args);
		}
	}
}

void CSTrackingManager::vCopy(CSTrackingManager const* manager)
{
	for (int32 i = 0; i < manager->iData.Num(); i++)
	{
		TrackSound(manager->iData.vGetID(i), manager->iData.vGetItem(i).pSound);
	}
}

void CSTrackingManager::iNotifyOfNewTrackedSound(FCSSoundID const& id)
{
	FDelegateHandle handle;
	if (iPendingIndicatorDelegates.RemoveAndCopyValue(id, handle))
	{
		NewSoundTrackedEvent.Broadcast(id);
		NewSoundTrackedEvent.Remove(handle);
	}
}
