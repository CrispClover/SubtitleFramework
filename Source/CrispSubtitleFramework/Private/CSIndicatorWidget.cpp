// Copyright Crisp Clover.

#include "CSIndicatorWidget.h"
#include "CSS_SubtitleGISS.h"
#include "CSUserSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Slate/SGameLayerManager.h"

#if WITH_EDITOR
void UCSIndicatorWidget::eConstructExample(FVector2D const& size)
{
	Super::eConstructExample(size);
	
	if (!Image)
		return;

	UCSUserSettings* settings = UCSProjectSettingFunctions::GetDesignSettings(size);
	Image->SetDesiredSizeOverride(settings->GetLayout().IndicatorSize);
}
#endif

void UCSIndicatorWidget::NativeConstruct()
{
	uCSS = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UCSS_SubtitleGISS>();

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
		uCSS->UnregisterIndicator(iSoundID, GetOwningLocalPlayer(), this);

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
	Image->SetDesiredSizeOverride(uCSS->GetCurrentSettings()->GetLayout().IndicatorSize);//Our change in position could be due to a resized widow.
	iUpdateOffset();
}

void UCSIndicatorWidget::OnUpdateIndicators_Implementation()
{
	if (!uWidgetData)
		return;

	Image->SetRenderTransformAngle(UCSCoreLibrary::AngleConversion(uWidgetData->Angle, Segments));
}

void UCSIndicatorWidget::Register_Implementation(FCSSoundID const& id)
{
	iSoundID = id;

	if (!uCSS)
		return;

	CSIndicatorFunctions<UCSIndicatorWidget> funcs
		= CSIndicatorFunctions<UCSIndicatorWidget>
		(
			  &UCSIndicatorWidget::OnUpdateIndicators
			, &UCSIndicatorWidget::iUpdateDataPtr
			, &UCSIndicatorWidget::uRegister
		);

	CSIndicatorRegistrationData<UCSIndicatorWidget> args = CSIndicatorRegistrationData<UCSIndicatorWidget>(this, CSRegisterArgs(id, uWidgetData), funcs);

	uCSS->RegisterIndicator(args, GetOwningLocalPlayer());

	Image->SetDesiredSizeOverride(uCSS->GetCurrentSettings()->GetLayout().IndicatorSize);//TODO: move?
}

void UCSIndicatorWidget::uRegister(FCSSoundID const& id)
{
	if (id != iSoundID)
		return;

	CSIndicatorFunctions<UCSIndicatorWidget> funcs
		= CSIndicatorFunctions<UCSIndicatorWidget>
		(
			  &UCSIndicatorWidget::OnUpdateIndicators
			, &UCSIndicatorWidget::iUpdateDataPtr
			, &UCSIndicatorWidget::uRegister
		);

	CSIndicatorRegistrationData<UCSIndicatorWidget> args = CSIndicatorRegistrationData<UCSIndicatorWidget>(this, CSRegisterArgs(id, uWidgetData), funcs);

	uCSS->RegisterIndicator(args, GetOwningLocalPlayer());
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
	
	FGeometry const& layerGeo = gameLayerManager->GetPlayerWidgetHostGeometry(player);
	FVector2D layerSize = layerGeo.GetAbsoluteSize();

	uWidgetData->Offset = UCSCoreLibrary::LocalPositionToNDC(layerGeo.AbsoluteToLocal(iCenterPos), viewportSize.IntPoint(), layerSize);
}

void UCSIndicatorWidget::iUpdateDataPtr(CSSwapArgs const& swapArgs)
{
	if (swapArgs.ID != iSoundID)
		return;

	uWidgetData = swapArgs.WidgetDataPtr;
}