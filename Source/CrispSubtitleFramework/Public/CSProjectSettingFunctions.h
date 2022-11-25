// Copyright Crisp Clover.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CSProjectSettings.h"
#include "CSCoreLibrary.h"
#include "CSProjectSettingFunctions.generated.h"

class UCSUserSettings;

/**
 * 
 */
UCLASS()
class CRISPSUBTITLEFRAMEWORK_API UCSProjectSettingFunctions : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	//Returns the maximum subtitles per container. Defined in the plugin's project settings.
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|ProjectSettings")
		static FORCEINLINE int32 GetMaxSubtitles()
			{ return GetDefault<UCSProjectSettings>()->MaximumSubtitles; };
	
	//Returns the maximum subtitles lines per container. Defined in the plugin's project settings.
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|ProjectSettings")
		static FORCEINLINE int32 GetMaxSubtitleLines()
			{ return GetDefault<UCSProjectSettings>()->MaximumSubtitleLines; };

	//Returns whether the subsystem should pause all queued subtitles when a permanent subtitle starts being displayed.
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|ProjectSettings")
		static FORCEINLINE bool ShouldPauseOnPermanentSubtitle()
			{ return GetDefault<UCSProjectSettings>()->bPauseOnPermanentSubtitle; };

	//Returns the settings path. Defined in the plugin's project settings.
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|ProjectSettings")
		static FORCEINLINE FString const& GetSettingsPath()
			{ return GetDefault<UCSProjectSettings>()->SettingsPath.Path; };

	//Returns the default settings. Defined in the plugin's project settings.
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|ProjectSettings")
		static FORCEINLINE UCSUserSettings* GetDefaultSettings()
			{ return GetDefault<UCSProjectSettings>()->DefaultSettings.LoadSynchronous(); };

	//Returns the default settings for design-time. Defined in the plugin's project settings.
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|ProjectSettings")
		static UCSUserSettings* GetDesignSettings();
	
	//Returns whether the project should support splitscreen direction indicators. Defined in the plugin's project settings.
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|ProjectSettings")
		static FORCEINLINE bool SupportSplitscreen()
			{ return GetDefault<UCSProjectSettings>()->bSupportSplitscreen; };

	//Returns the default FName used to track speech for direction indicators.
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|ProjectSettings")
		static FORCEINLINE FName GetSoundNameForSpeech()
			{ return GetDefault<UCSProjectSettings>()->DefaultSoundNameForSpeech; };

	//Returns whether the project should calculate direction indicator data on tick. Defined in the plugin's project settings.
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|ProjectSettings")
		static FORCEINLINE bool CalculateIndicatorsOnTick()
			{ return GetDefault<UCSProjectSettings>()->bCalculateIndicatorsOnTick; };

	//Returns whether the project requires captions to be displayed in the same UI as the subtitles.
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|ProjectSettings")
		static FORCEINLINE bool GetDisplayCaptionsAsSubtitles()
			{ return GetDefault<UCSProjectSettings>()->bDisplayCaptionsAsSubtitles; };

	//Returns the example subtitle defined in the plugin's project settings.
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|ProjectSettings")
		static FCrispSubtitle GetExampleSubtitle(UCSUserSettings const* Settings);

	//Returns the example subtitle with the default project settings applied, both defined in the plugin's project settings.
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|ProjectSettings")
		static FORCEINLINE FCrispSubtitle GetDefaultExampleSubtitle()
			{ return GetExampleSubtitle(GetDefaultSettings()); };

	//Returns the example caption defined in the plugin's project settings.
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|ProjectSettings")
		static FCrispCaption GetExampleCaption();
};
