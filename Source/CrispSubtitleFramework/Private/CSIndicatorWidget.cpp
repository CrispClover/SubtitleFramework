// Copyright Crisp Clover.

#include "CSIndicatorWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Slate/SGameLayerManager.h"

void UCSIndicatorWidget::NativeConstruct()
{
	oCSS = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UCSS_SubtitleGISS>();

	Super::NativeConstruct();
}

void UCSIndicatorWidget::NativeTick(FGeometry const& myGeometry, float deltaTime)//TODO
{
	UpdateCenterPosition(myGeometry);

	Super::NativeTick(myGeometry, deltaTime);
}

void UCSIndicatorWidget::NativeDestruct()
{
	if (iWidgetData)
		oCSS->UnregisterIndicator(iSoundID, GetOwningLocalPlayer(), this);

	Super::NativeDestruct();
}

bool UCSIndicatorWidget::GetIndicatorData(FCSIndicatorWidgetData& WidgetData) const
{
	if (!iWidgetData)
		return false;

	WidgetData = *iWidgetData;
	return true;
}

void UCSIndicatorWidget::UpdateCenterPosition(FGeometry const& myGeometry)//TODO: all of it.
{
	FVector2D const& newCenterPos = myGeometry.GetAbsolutePositionAtCoordinates(FVector2D(.5f, .5f));

	if (newCenterPos == iCenterPos)
		return;

	iCenterPos = newCenterPos;
	Image->SetDesiredSizeOverride(oCSS->GetCurrentSettings()->GetLayout().IndicatorSize);//Our change in position could be due to a resized widow.
	iUpdateOffset();
}

void UCSIndicatorWidget::OnUpdateIndicators_Implementation()
{
	if (!iWidgetData)
		return;

	Image->SetRenderTransformAngle(UCSLibrary::AngleConversion(iWidgetData->Angle, Segments));

	if (iWidgetData < 0)
		Image->SetRenderOpacity(1);
	else
		Image->SetRenderOpacity(iWidgetData->OpacityDriver * 10 - .75f);//TODO: move to BPs?
}

void UCSIndicatorWidget::Register_Implementation(FCSSoundID const& id)
{
	iSoundID = id;
	
	if (!oCSS)
		return;

	CSIndicatorDelegates* delegates = oCSS->RegisterIndicator(FCSRegisterArgs(iSoundID, iWidgetData), GetOwningLocalPlayer());
	
	if (!delegates)
		return;
	
	delegates->UpdateIDataEvent.AddUObject(this, &UCSIndicatorWidget::OnUpdateIndicators);
	delegates->SwapIDataEvent.AddUObject(this, &UCSIndicatorWidget::iUpdateDataPtr);

	Image->SetDesiredSizeOverride(oCSS->GetCurrentSettings()->GetLayout().IndicatorSize);//TODO: move?
}

void UCSIndicatorWidget::iUpdateOffset() const
{
	if (!iWidgetData)
		return;

	ULocalPlayer* const player = GetOwningLocalPlayer();
	if (!player || !player->ViewportClient)
		return;

	FVector2D viewportSize;
	player->ViewportClient->GetViewportSize(viewportSize);

	TSharedPtr<IGameLayerManager> gameLayerManager = player->ViewportClient->GetGameLayerManager();
	if (!gameLayerManager.IsValid())
		return;

	FGeometry const& viewportGeo = gameLayerManager->GetViewportWidgetHostGeometry();
	iWidgetData->Offset = UCSLibrary::LocalPositionToNDC(viewportGeo.AbsoluteToLocal(iCenterPos), viewportSize.IntPoint());
}

void UCSIndicatorWidget::iUpdateDataPtr(FCSSwapArgs const& swapArgs)
{
	if (swapArgs.ID != iSoundID)
		return;

	iWidgetData = swapArgs.WidgetDataPtr;
}