// Copyright Crisp Clover.

#include "CSIndicatorWidget.h"
#include "CSS_SubtitleGISS.h"
#include "CSUserSettings.h"
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
	if (uWidgetData)
		oCSS->UnregisterIndicator(iSoundID, GetOwningLocalPlayer(), this);

	Super::NativeDestruct();
}

bool UCSIndicatorWidget::GetIndicatorData(FCSIndicatorWidgetData& WidgetData) const
{
	if (!uWidgetData)
		return false;

	WidgetData = *uWidgetData;
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
	if (!uWidgetData)
		return;

	Image->SetRenderTransformAngle(UCSLibrary::AngleConversion(uWidgetData->Angle, Segments));

	if (uWidgetData->OpacityDriver < 0)
		Image->SetRenderOpacity(1);
	else
		Image->SetRenderOpacity(uWidgetData->OpacityDriver * 10 - .75f);//TODO: move to BPs?
}

void UCSIndicatorWidget::Register_Implementation(FCSSoundID const& id, float scaling)
{
	iSoundID = id;

	if (!oCSS)
		return;

	CSIndicatorDelegates* delegates = oCSS->rRegisterIndicator(FCSRegisterArgs(iSoundID, uWidgetData), GetOwningLocalPlayer());
	
	if (!delegates)
		return;

	delegates->UpdateIDataEvent.AddUObject(this, &UCSIndicatorWidget::OnUpdateIndicators);
	delegates->SwapIDataEvent.AddUObject(this, &UCSIndicatorWidget::iUpdateDataPtr);

	Image->SetDesiredSizeOverride(oCSS->GetCurrentSettings()->GetLayout().IndicatorSize * scaling);//TODO: move?
}

void UCSIndicatorWidget::iUpdateOffset() const
{
	if (!uWidgetData)
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
	uWidgetData->Offset = UCSLibrary::LocalPositionToNDC(viewportGeo.AbsoluteToLocal(iCenterPos), viewportSize.IntPoint());
}

void UCSIndicatorWidget::iUpdateDataPtr(FCSSwapArgs const& swapArgs)
{
	if (swapArgs.ID != iSoundID)
		return;

	uWidgetData = swapArgs.WidgetDataPtr;
}