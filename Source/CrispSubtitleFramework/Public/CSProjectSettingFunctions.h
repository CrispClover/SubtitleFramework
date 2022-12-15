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

#if WITH_EDITOR
public:
	//Returns the default settings for design-time. Defined in the plugin's project settings.
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|ProjectSettings")
		static UCSUserSettings* GetDesignSettings(FVector2D const& ScreenSize);
#endif

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
		static TArray<FString> GetSettingsDirectories();

	//Returns the default settings. Defined in the plugin's project settings.
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|ProjectSettings")
		static FORCEINLINE UCSUserSettings* GetDefaultSettings()
			{ return GetDefault<UCSProjectSettings>()->DefaultSettings.LoadSynchronous(); };

	//Returns whether the project should support splitscreen direction indicators. Defined in the plugin's project settings.
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|ProjectSettings")
		static FORCEINLINE bool SupportSplitscreen()
			{ return GetDefault<UCSProjectSettings>()->bSupportSplitscreen; };

	//Returns the default FName used to identify captions delivered to the subtitle UI.
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|ProjectSettings")
		static FORCEINLINE FName GetSpeakerNameForCaptions()
			{ return GetDefault<UCSProjectSettings>()->SpeakerNameForCaptions; };

	//Returns the default FName used to track speech for direction indicators.
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|ProjectSettings")
		static FORCEINLINE FName GetSoundNameForSpeech()
			{ return GetDefault<UCSProjectSettings>()->SoundNameForSpeech; };

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
		static TArray<FCrispSubtitle> GetExampleSubtitles(UCSUserSettings const* Settings);

	//Returns the example subtitle with the default project settings applied, both defined in the plugin's project settings.
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|ProjectSettings")
		static FORCEINLINE TArray<FCrispSubtitle> GetDefaultExampleSubtitle()
			{ return GetExampleSubtitles(GetDefaultSettings()); };

	//Returns the example caption defined in the plugin's project settings.
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|ProjectSettings")
		static TArray<FCrispCaption> GetExampleCaptions();
};
