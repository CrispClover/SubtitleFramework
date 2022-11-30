// Copyright Crisp Clover.

#include "CSProjectSettings.h"
#include "CSUserSettings.h"

UCSProjectSettings::UCSProjectSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, bDisplayCaptionsAsSubtitles(false)
	, MaximumSubtitles(3)
	, MaximumSubtitleLines(5)
	, bSupportSplitscreen(false)
	, bCalculateIndicatorsOnTick(true)
	, bPauseOnPermanentSubtitle(false)
	, SpeakerNameForCaptions(FName("caption"))
	, SoundNameForSpeech(FName("speech"))
	, ExampleSubtitle
	(
		  FRawSubtitle
		  (
			  FSoundCaption(FText::FromString("testing"))
			, TArray<FText>{ FText::FromString("With the currently selected settings"), FText::FromString("subtitles will look like this.") }
			, 3
		  )
		, FText::FromString("Test")
		, FName("test")
		, FName("test")
	)
	, ExampleCaption
	(
		  FSoundCaption(FText::FromString("Click"))
		, FName("cursor")
		, FName("click")
	)
{}