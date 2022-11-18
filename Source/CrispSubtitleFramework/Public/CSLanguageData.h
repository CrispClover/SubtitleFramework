// Copyright Crisp Clover.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CSLanguageData.generated.h"

/*
 * 
 */
UCLASS(BlueprintType)
class CRISPSUBTITLEFRAMEWORK_API UCSLanguageData : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CrispSubtitles")
		float MinSubtitleTime = .85f;//TODO: global?

	/**
	 * Estimates:
	 * .375 for English;
	 * .542 for Chinese;
	 * .442 for Japanese;
	 * .47 for Thai;
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CrispSubtitles")
		float WordTime = .375f;

	/**
	 * Estimates:
	 * .087 for English;
	 * .335 for Chinese;
	 * .239 for Japanese;
	 * .134 for Thai;
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CrispSubtitles")
		float CharacterTime = .087f;
};
