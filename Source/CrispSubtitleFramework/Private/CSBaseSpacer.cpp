// Copyright Crisp Clover.

#include "CSBaseSpacer.h"
#include "Components/Spacer.h"

void UCSBaseSpacer::SetSize_Implementation(FVector2D const& size)
{
	if (Spacer)
		Spacer->SetSize(size);
}