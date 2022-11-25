// Copyright Crisp Clover.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CSCustomDataManager.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class CRISPSUBTITLEFRAMEWORK_API UCSCustomDataManager : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, Category = "CrispSubtitles|CustomData")
		void RemoveData(const int32 ID);
};
