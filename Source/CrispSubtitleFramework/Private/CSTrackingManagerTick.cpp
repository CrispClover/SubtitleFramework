// Copyright Crisp Clover.

#include "CSTrackingManagerTick.h"

void CSTrackingManagerTick::Tick( float DeltaTime )
{
	if (infTicked == GFrameCounter)
		return;

	Calculate();

	infTicked = GFrameCounter;
}