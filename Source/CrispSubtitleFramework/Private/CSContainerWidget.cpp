// Copyright Crisp Clover.

#include "CSContainerWidget.h"
#include "CSS_SubtitleGISS.h"
#include "Kismet/GameplayStatics.h"

void UCSContainerWidget::NativeConstruct()
{
	uCSS = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UCSS_SubtitleGISS>();

	Super::NativeConstruct();
}