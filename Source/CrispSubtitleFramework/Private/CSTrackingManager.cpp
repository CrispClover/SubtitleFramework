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
		FCSSoundIndicatorData& sound3D = iData.AccessItem(i);

		const FVector4 projection = projectionMatrix.TransformFVector4(FVector4(sound3D.SoundData, 1.f));
		const float m = 1 - 2 * (int8)(projection.W < 0);//We want to mirror coordinates behind the player's view.

		//Normalised coordinates (X/W), offset subtracted, and mirrored in back:
		const FVector2f adjustedNDC = m * FVector2f(projection.X / projection.W - sound3D.Offset.X, -projection.Y / projection.W - sound3D.Offset.Y);

		sound3D.Angle = FMath::Atan2(adjustedNDC.Y * rectangle.Height(), adjustedNDC.X * rectangle.Width());
		sound3D.OpacityDriver = m * adjustedNDC.Length();//Negative numbers were unused. "Packing" projection.W < 0.
	}

	/* TODO: 2D
	for (int32 i = 0; data2D.NeedsCalc(i); i++)
	{
		FCSSoundData<FVector2D>& sound2D = data2D.AccessItem(i);

		//UCSCoreLibrary::LocalPositionToNDC(source2D.SoundPosition, rectangle);//TODO: how to convert specific splitscreen viewport?

		const FVector2f offsetNDC = FVector2f(sound2D.SoundData.X - sound2D.Offset.X, -sound2D.SoundData.Y - sound2D.Offset.Y);//TODO: behaviour splitscreen?

		sound2D.Angle = FMath::Atan2(offsetNDC.Y * rectangle.Height(), offsetNDC.X * rectangle.Width());
		sound2D.OpacityDriver = offsetNDC.Length();
	}*/

	iDelegates.UpdateIDataEvent.Broadcast();
}

bool CSTrackingManager::GetSoundData(FCSSoundID const& id, FVector& data) const
{
	FCSSoundIndicatorData const* ptr = iData.rFind(id);

	if (!ptr)
		return false;

	data = ptr->SoundData;
	return true;
}

CSIndicatorDelegates* CSTrackingManager::rRegisterIndicator(FCSRegisterArgs args)
{
	if (iData.Register(args.ID, args.WidgetDataPtrRef))
		return &iDelegates;
	else
		return nullptr;
}

void CSTrackingManager::UnregisterIndicator(FCSSoundID const& id, UObject* widget)
{
	iDelegates.Remove(widget);

	FCSSwapArgs args = FCSSwapArgs(id, nullptr);

	iData.Unregister(id, args);

	if (args.WidgetDataPtr)
		iDelegates.SwapIDataEvent.Broadcast(args);
}

void CSTrackingManager::Copy(CSTrackingManager const* manager)
{
	for (int32 i = 0; i < manager->iData.Num(); i++)
		TrackSound(manager->iData.GetID(i), manager->iData.GetItem(i).SoundData);
}
