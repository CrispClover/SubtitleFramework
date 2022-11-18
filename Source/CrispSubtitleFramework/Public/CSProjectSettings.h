// Copyright Crisp Clover.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CSLibrary.h"
#include "CSProjectSettings.generated.h"

class UCSCustomDataManager;
class UCSUserSettings;

/**
 * 
 */
UCLASS(config = Engine, defaultconfig)
class CRISPSUBTITLEFRAMEWORK_API UCSProjectSettings : public UObject
{
	GENERATED_UCLASS_BODY()

public:
	//The settings asset to use by default.
	UPROPERTY(EditAnywhere, config, Category = "UserSettings")
		TSoftObjectPtr<UCSUserSettings> DefaultSettings;

	//The paths to search for settings assets.
	UPROPERTY(EditAnywhere, config, Category = "UserSettings", meta = (ContentDir))
		FDirectoryPath SettingsPath;

	//Whether to display sound captions inside the subtitle container
	UPROPERTY(EditAnywhere, config, Category = "UI")
		bool bDisplayCaptionsAsSubtitles;

	//The number of subtitles allowed in the subtitle container.
	UPROPERTY(EditAnywhere, config, Category = "UI", meta = (UIMin = 1, UIMax = 4))
		int32 MaximumSubtitles;

	//The number of subtitles allowed in the subtitle container.
	UPROPERTY(EditAnywhere, config, Category = "UI", meta = (UIMin = 3, UIMax = 8))
		int32 MaximumSubtitleLines;

	//The settings asset to use by default.
	UPROPERTY(EditAnywhere, config, Category = "UserSettings")
		TSubclassOf<UCSCustomDataManager> CustomDataClass;

	//Whether to support splitscreen captions
	UPROPERTY(EditAnywhere, config, Category = "Project")
		bool bSupportSplitscreen;

	//Whether to calculate direction indicator data on tick
	UPROPERTY(EditAnywhere, config, Category = "Project", meta = (ConfigRestartRequired = true))
		bool bCalculateIndicatorsOnTick;

	//Whether single subtitles with multiple speakers exist in the project
	UPROPERTY(EditAnywhere, config, Category = "Project")//TODO?
		bool bMergedSubtitlesExist;

	/*
	 * If merged subtitles exist, this tells the subsystem how to spot them.
	 * (The subtitle lines in question should begin with these characters.)
	 */
	UPROPERTY(EditAnywhere, config, Category = "Project")
		FString MergedSubtitlePrefix;//TODO

	UPROPERTY(EditAnywhere, config, Category = "Examples")
		FFullSubtitle ExampleSubtitle;

	UPROPERTY(EditAnywhere, config, Category = "Examples")
		FFullCaption ExampleCaption;
};
