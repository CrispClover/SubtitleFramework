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
				FSoundCaption(NSLOCTEXT("CrispSubtitles", "testing", "testing"))
				, TArray<FText>
				{
					  NSLOCTEXT("CrispSubtitles", "exampleSubLine0", "With the currently selected settings")
					, NSLOCTEXT("CrispSubtitles", "exampleSubLine1", "subtitles will look like this.")
				}
				, 3
			)
			, NSLOCTEXT("CrispSubtitles", "test", "Test")
			, FName("test")
			, FName("test")
		)
	})
	, ExampleCaptions
	({
		  FFullCaption(FSoundCaption(NSLOCTEXT("CrispSubtitles", "example", "Example")), FName("example"), FName("example"))
		, FFullCaption(FSoundCaption(NSLOCTEXT("CrispSubtitles", "test", "Test")), FName("example"), FName("test"))
	})
{
#if WITH_EDITOR
	if (SettingsDirectories.IsEmpty())
	{
		FDirectoryPath settings;
		settings.Path = "/CrispSubtitleFramework/Settings";
		SettingsDirectories.Add(settings);
	}
#endif
}