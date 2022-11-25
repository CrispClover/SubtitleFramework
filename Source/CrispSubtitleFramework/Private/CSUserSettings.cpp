// Copyright Crisp Clover.

#include "CSUserSettings.h"
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
	//Label:
	, FullLabelFormat(FText::FromString("{speaker}: [{description}]"))
	, SpeakerOnlyLabelFormat(FText::FromString("{speaker}:"))
	, DescriptionOnlyLabelFormat(FText::FromString("[{description}]"))
	, ShowSpeaker(EShowSpeakerType::Always)
	, bSpeakersAreUpperCase(true)
	, bShowSubtitleDescriptions(false)
	//Font:
	, Font(nullptr)
	, RegularTypeface(FName("Regular"))
	, ItalicTypeface(FName("Italic"))
	, CaptionTypeface(FName("Regular"))
	, Outline(FFontOutlineSettings())
	, LetterSpacing(0)
	, FontMaterial(nullptr)
	, SubtitleTextSize(.04f)
	, CaptionTextSize(.04f)
	//Colours:
	, ColourCoding(EColourCodeType::None)
	, DefaultTextColour(FLinearColor::White)
	, AssignedTextColours(TMap<FName, FLinearColor>())
	, AvailableTextColours(TArray<FLinearColor>{FLinearColor::White, FLinearColor::Yellow, FLinearColor(0, 1, 1), FLinearColor::Green})
	, LetterboxColour(FLinearColor::FLinearColor(0, 0, 0, .5f))
	, LineBackColour(FLinearColor::Transparent)
	, CaptionBackColour(FLinearColor::FLinearColor(0, 0, 0, .5f))
	//Layout:
	, SubtitlePadding(.015f)
	, LinePadding(0.f)
	, CaptionPadding(0.01f)
	, IndicatorSize(1.0f)
	//Timing:
	, ReadingSpeed(1.f)
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

	if (iCachedLayout.BaseSize == size)
		return;

	iCachedLayout.BaseSize = size;
	iRecalculateLayout();
}

void UCSUserSettings::RecalculateDesignLayout(const FIntPoint screenSize)
{
	const int32 size = FMath::Min(screenSize.X, screenSize.Y);

	if (iCachedLayout.BaseSize == size)
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
	iCachedLayout.FontInfo.TypefaceFontName = RegularTypeface;//TODO?

	iCachedLayout.CaptionTextSize = iCachedLayout.BaseSize * CaptionTextSize;

	iCachedLayout.TextPadding = FMargin(iCachedLayout.FontInfo.Size * .5f, iCachedLayout.FontInfo.Size * .25f);
	iCachedLayout.LinePadding = FMargin(0, iCachedLayout.BaseSize * LinePadding, 0, 0);
	iCachedLayout.SubtitlePadding = FMargin(0, iCachedLayout.BaseSize * SubtitlePadding, 0, 0);
	iCachedLayout.BoxPadding = FMargin(iCachedLayout.BaseSize * .03f, iCachedLayout.BaseSize * .01f);
	iCachedLayout.CaptionPadding = FMargin(0, iCachedLayout.BaseSize * CaptionPadding, 0, 0);

	iCachedLayout.IndicatorSize = FVector2D(iCachedLayout.CaptionTextSize * IndicatorSize);
}

bool UCSUserSettings::GetShowSpeaker(const FName speakerID) const
{
	switch (ShowSpeaker)
	{
	case EShowSpeakerType::ColourCoded:
		return true/*TODO*/;

	case EShowSpeakerType::Never:
		return false;

	default:
		return true;
	}
}

FLinearColor const& UCSUserSettings::GetTextColour(FName speakerID) const
{
	switch (ColourCoding)
	{
	case EColourCodeType::Assigned:
		if (!AssignedTextColours.Contains(speakerID))
		{
			if (AvailableTextColours.Num() > 0)
				return AvailableTextColours.Last();
			else
				return DefaultTextColour;
		}
		else if (AssignedTextColours.Contains(speakerID) /*&& shown.Contains(speakerID)*/)//TODO
		{
			return *AssignedTextColours.Find(speakerID);
		}
		else if (AssignedTextColours.Contains(speakerID) /*&& shown.Contains(speakerID)*/)//TODO
		{
			//shown.Add(speakerID);
			return *AssignedTextColours.Find(speakerID);
		}
		else if (AvailableTextColours.Num() > 0)
		{
			return AvailableTextColours.Last();
		}
		else
		{
			return DefaultTextColour;
		}

	case EColourCodeType::TODO:
		//TODO
		break;

	case EColourCodeType::None:
		if (AvailableTextColours.Num() > 0)
			return AvailableTextColours[0];
		break;

	default:
		return FLinearColor::White;
	}
	return FLinearColor::White;
}

TArray<FName> UCSUserSettings::GetTypefaceOptions() const
{
	TArray<FName> options = TArray<FName>();

	if (UFont const* font = Cast<const UFont>(Font.LoadSynchronous()))
		for (const FTypefaceEntry& typefaceEntry : font->CompositeFont.DefaultTypeface.Fonts)
			options.Add(typefaceEntry.Name);

	return options;
}