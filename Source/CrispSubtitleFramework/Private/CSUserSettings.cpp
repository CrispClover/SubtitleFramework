// Copyright Crisp Clover.

#include "CSUserSettings.h"
#include "CSColourProfile.h"
#include "Engine/Font.h"

UCSUserSettings::UCSUserSettings()
	//Core:
	: DisplayName(FText::FromString("Default"))
	, LetterboxClass(nullptr)
	, LineClass(nullptr)
	, CaptionClass(nullptr)
	, bShowSubtitles(true)
	, bShowSubtitleIndicators(false)
	, bShowCaptions(false)
	, bShowCaptionIndicators(true)
	, ColourProfile(nullptr)
	//Label:
	, FullLabelFormat(FText::FromString("{speaker}: [{description}]"))
	, SpeakerOnlyLabelFormat(FText::FromString("{speaker}:"))
	, DescriptionOnlyLabelFormat(FText::FromString("[{description}]"))
	, ShowSpeaker(EShowSpeaker::Never)
	, bSpeakersAreUpperCase(true)
	, bShowSubtitleDescriptions(false)
	//Font:
	, Font(nullptr)
	, RegularTypeface(FName("Regular"))
	, IndirectSpeechTypeface(FName("Italic"))
	, CaptionTypeface(FName("Regular"))
	, Outline(FFontOutlineSettings())
	, LetterSpacing(0)
	, FontMaterial(nullptr)
	, SubtitleTextSize(.04f)
	, CaptionTextSize(.04f)
	//Layout:
	, SubtitlePadding(.015f)
	, LinePadding(0.f)
	, CaptionPadding(0.01f)
	, CaptionAlignment(EHorizontalAlignment::HAlign_Right)
	, IndicatorSize(1.0f)
	//Timing:
	, ReadingSpeed(1.f)
	, bAccumulateReadTime(true)
	, TimeGap(.16f)
	, MinimumSubtitleTime(1.f)
	, MinimumCaptionTime(.85f)
{};

void UCSUserSettings::RecalculateLayout(UGameViewportClient const* viewportClient)
{
	if (!viewportClient)
		return;

	FVector2D viewportSize;
	viewportClient->GetViewportSize(viewportSize);
	const int32 size = FMath::Min(viewportSize.X, viewportSize.Y);

	if (iCachedLayout.BaseSize == size || size <= 0)
		return;

	iCachedLayout.BaseSize = size;
	iRecalculateLayout();
}

void UCSUserSettings::RecalculateDesignLayout(FVector2D const& screenSize)
{
	const float size = FMath::Min(screenSize.X, screenSize.Y);
	
	if (iCachedLayout.BaseSize == size || size <= 0)
		return;

	iCachedLayout.BaseSize = size;
	iRecalculateLayout();
}

void UCSUserSettings::iRecalculateLayout()
{
	iCachedLayout.FontInfo.FontObject = Font.LoadSynchronous();
	iCachedLayout.FontInfo.OutlineSettings = Outline;
	iCachedLayout.FontInfo.LetterSpacing = LetterSpacing;
	iCachedLayout.FontInfo.FontMaterial = FontMaterial;
	iCachedLayout.FontInfo.Size = iCachedLayout.BaseSize * SubtitleTextSize;
	iCachedLayout.FontInfo.TypefaceFontName = RegularTypeface;

	iCachedLayout.CaptionTextSize = iCachedLayout.BaseSize * CaptionTextSize;

	iCachedLayout.TextPadding = FMargin(iCachedLayout.FontInfo.Size * .5f, iCachedLayout.FontInfo.Size * .25f);
	iCachedLayout.LinePadding = FMargin(0, iCachedLayout.BaseSize * LinePadding, 0, 0);
	iCachedLayout.SubtitlePadding = FMargin(0, iCachedLayout.BaseSize * SubtitlePadding, 0, 0);
	iCachedLayout.BoxPadding = FMargin(iCachedLayout.BaseSize * .03f, iCachedLayout.BaseSize * .01f);
	iCachedLayout.CaptionPadding = FMargin(0, iCachedLayout.BaseSize * CaptionPadding, 0, 0);

	iCachedLayout.IndicatorSize = FVector2D(iCachedLayout.CaptionTextSize * IndicatorSize);
}

bool UCSUserSettings::GetShowSpeaker(const FName speaker) const
{
	switch (ShowSpeaker)
	{
	case EShowSpeaker::ColourCodedShowOnce:
		if (UCSCPMatched* cp = Cast<UCSCPMatched>(ColourProfile.Get()))
			return !cp->ColourWasMatched(speaker);
		else
			return true;

	case EShowSpeaker::ColourCodedShowNever:
		if (UCSCPMatched* cp = Cast<UCSCPMatched>(ColourProfile.Get()))
			return !cp->HasColour(speaker);
		else
			return true;

	case EShowSpeaker::Never:
		return false;

	default:
		return true;
	}
}

void UCSUserSettings::LogSpeakerShown(const FName speaker) const
{
	if (UCSCPMatched* cp = Cast<UCSCPMatched>(ColourProfile.LoadSynchronous()))
		cp->LogMatch(speaker);
}

FLinearColor const& UCSUserSettings::GetSubtitleTextColour(const FName speaker) const
{
	if (ColourProfile.IsNull())
		return FLinearColor::White;
	else
		return ColourProfile.LoadSynchronous()->GetSubtitleColour(speaker);
}

FLinearColor const& UCSUserSettings::GetCaptionTextColour(const FName speaker) const
{
	if (ColourProfile.IsNull())
		return FLinearColor::White;
	else
		return ColourProfile.LoadSynchronous()->GetCaptionColour(speaker);
}

#if WITH_EDITOR
TArray<FName> UCSUserSettings::GetTypefaceOptions() const
{
	TArray<FName> options = TArray<FName>();

	if (UFont const* font = Cast<const UFont>(Font.LoadSynchronous()))
		for (const FTypefaceEntry& typefaceEntry : font->CompositeFont.DefaultTypeface.Fonts)
			options.Add(typefaceEntry.Name);

	return options;
}
#endif