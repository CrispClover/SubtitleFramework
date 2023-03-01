// Copyright Crisp Clover.

#include "CSProjectSettings.h"
#include "CSUserSettings.h"

UCSProjectSettings::UCSProjectSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, DefaultSettings(FSoftObjectPath("/CrispSubtitleFramework/Settings/DefaultSettings.DefaultSettings"))
	, SettingsDirectories()
	, bDisplayCaptionsAsSubtitles(false)
	, MaximumSubtitles(3)
	, MaximumSubtitleLines(5)
	, bSupportSplitscreen(false)
	, bCalculateIndicatorsOnTick(true)
	, bPauseOnPermanentSubtitle(false)
	, SpeakerNameForCaptions(FName("caption"))
	, SoundNameForSpeech(FName("speech"))
	, ExampleSubtitles
	({
		FFullSubtitle
		(
			FRawSubtitle
			(
				FSoundCaption(FText::FromString("testing"))
				, TArray<FText>{FText::FromString("With the currently selected settings"), FText::FromString("subtitles will look like this.") }
				, 3
			)
			, FText::FromString("Test")
			, FName("test")
			, FName("test")
		)
	})
	, ExampleCaptions
	({
		  FFullCaption(FSoundCaption(FText::FromString("Example")), FName("example"), FName("example"))
		, FFullCaption(FSoundCaption(FText::FromString("Test")), FName("example"), FName("test"))
	})
{
#if WITH_EDITOR
	if (SettingsDirectories.Num() == 0)
	{
		FDirectoryPath settings;
		settings.Path = "/CrispSubtitleFramework/Settings";
		SettingsDirectories.Add(settings);
	}
#endif
}