// Copyright Crisp Clover.

#include "CSUILibrary.h"
#include "CSUserSettings.h"
#include "CSColourProfile.h"
#include "CSProjectSettingFunctions.h"

#pragma region STRUCTS
FCSLineStyle::FCSLineStyle()
    : FontInfo()
    , TextColour(FLinearColor::White)
    , BackColour(FLinearColor::Black)
    , TextPadding()
{
	static const FName SpecialName_DefaultSystemFont("DefaultSystemFont");
    FSlateFontInfo fontInfo = FSlateFontInfo();

};

FCSLetterboxStyle::FCSLetterboxStyle()
    : LineClass(UCSProjectSettingFunctions::GetDefaultSettings()->LineClass.LoadSynchronous())
    , LabelStyle()
    , LineStyle()
    , LetterboxColour(FLinearColor::Black)
    , BoxPadding()
    , LinePadding()
    , bShowIndicator(true)
{};

FCSCaptionStyle::FCSCaptionStyle()
    : FontInfo()
    , TextColour(FLinearColor::White)
    , BackColour(FLinearColor::Black)
    , TextPadding()
    , bShowIndicator(true)
{};
#pragma endregion

FCSLetterboxStyle UCSUILibrary::GetLetterboxStyle(UCSUserSettings const* settings, const FName speaker, const bool indirectSpeech)
{
    if (!settings)
        return FCSLetterboxStyle();

    FLayoutCacheData const& layout = settings->GetLayout();

    FCSLetterboxStyle style = FCSLetterboxStyle();

    style.LetterboxColour = settings->ColourProfile.LoadSynchronous()->LetterboxColour;
    style.LineClass = settings->LineClass.LoadSynchronous();
    style.LineStyle = GetLabelStyle(settings, speaker);
    style.LineStyle = GetLineStyle(settings, speaker, indirectSpeech);
    style.BoxPadding = layout.BoxPadding;
    style.LinePadding = layout.LinePadding;
    style.bShowIndicator = settings->bShowSubtitleIndicators;

    return style;
}

FCSLineStyle UCSUILibrary::GetLabelStyle(UCSUserSettings const* settings, const FName speaker)
{
    if (!settings)
        return FCSLineStyle();

    FLayoutCacheData const& layout = settings->GetLayout();

    FCSLineStyle style = FCSLineStyle();
    style.FontInfo = layout.FontInfo;
    style.BackColour = settings->ColourProfile.LoadSynchronous()->LineBackColour;
    style.TextColour = settings->GetSubtitleTextColour(speaker);
    style.TextPadding = layout.TextPadding;

    return style;
}

FCSLineStyle UCSUILibrary::GetLineStyle(UCSUserSettings const* settings, const FName speaker, const bool indirectSpeech)
{
    if (!settings)
        return FCSLineStyle();

    FLayoutCacheData const& layout = settings->GetLayout();

    FCSLineStyle style = FCSLineStyle();
    style.FontInfo = layout.FontInfo;

    if (indirectSpeech)
    {
        style.FontInfo.TypefaceFontName = settings->IndirectSpeechTypeface;
    }

    style.BackColour = settings->ColourProfile.LoadSynchronous()->LineBackColour;
    style.TextColour = settings->GetSubtitleTextColour(speaker);
    style.TextPadding = layout.TextPadding;

    return style;
}

FCSCaptionStyle UCSUILibrary::GetCaptionStyle(UCSUserSettings const* settings, const FName source)
{
    if (!settings)
        return FCSCaptionStyle();

    FLayoutCacheData const& layout = settings->GetLayout();

    FCSCaptionStyle style = FCSCaptionStyle();
    style.FontInfo = layout.FontInfo;
    style.FontInfo.Size = layout.CaptionTextSize;
    style.FontInfo.TypefaceFontName = settings->CaptionTypeface;
    style.BackColour = settings->ColourProfile.LoadSynchronous()->CaptionBackColour;
    style.TextColour = settings->GetCaptionTextColour(source);
    style.TextPadding = layout.TextPadding;

    return style;
}

#if WITH_EDITOR
FCSLetterboxStyle UCSUILibrary::GetDesignLetterboxStyle(const FName speaker, const bool isIndirectSpeech, FVector2D const& screenSize)
{
    UCSUserSettings* settings = UCSProjectSettingFunctions::GetDesignSettings(screenSize);

    if (!settings)
        return FCSLetterboxStyle();
    
    FLayoutCacheData const& layout = settings->GetLayout();

    FCSLetterboxStyle style = FCSLetterboxStyle();
    style.LetterboxColour = settings->ColourProfile.LoadSynchronous()->LetterboxColour;
    style.LineClass = settings->LineClass.LoadSynchronous();
    style.BoxPadding = layout.BoxPadding;
    style.LinePadding = layout.LinePadding;
    style.LabelStyle = GetDesignLabelStyle(speaker, screenSize);
    style.LineStyle = GetDesignLineStyle(speaker, isIndirectSpeech, screenSize);
    style.bShowIndicator = settings->bShowSubtitleIndicators;

    return style;
}

FCSLineStyle UCSUILibrary::GetDesignLabelStyle(const FName speaker, FVector2D const& screenSize)
{
    UCSUserSettings* settings = UCSProjectSettingFunctions::GetDesignSettings(screenSize);

    if (!settings)
        return FCSLineStyle();
    
    FLayoutCacheData const& layout = settings->GetLayout();

    FCSLineStyle style = FCSLineStyle();
    style.FontInfo = layout.FontInfo;
    style.BackColour = settings->ColourProfile.LoadSynchronous()->LineBackColour;
    style.TextColour = settings->GetSubtitleTextColour(speaker);
    style.TextPadding = layout.TextPadding;

    return style;
}

FCSLineStyle UCSUILibrary::GetDesignLineStyle(const FName speaker, const bool isIndirectSpeech, FVector2D const& screenSize)
{
    UCSUserSettings* settings = UCSProjectSettingFunctions::GetDesignSettings(screenSize);

    if (!settings)
        return FCSLineStyle();
    
    FLayoutCacheData const& layout = settings->GetLayout();

    FCSLineStyle style = FCSLineStyle();
    style.FontInfo = layout.FontInfo;

    if (isIndirectSpeech)
    {
        style.FontInfo.TypefaceFontName = settings->IndirectSpeechTypeface;
    }

    style.BackColour = settings->ColourProfile.LoadSynchronous()->LineBackColour;
    style.TextColour = settings->GetSubtitleTextColour(speaker);
    style.TextPadding = layout.TextPadding;

    return style;
}

FCSCaptionStyle UCSUILibrary::GetDesignCaptionStyle(const FName source, FVector2D const& screenSize)
{
    UCSUserSettings* settings = UCSProjectSettingFunctions::GetDesignSettings(screenSize);

    if (!settings)
        return FCSCaptionStyle();

    FLayoutCacheData const& layout = settings->GetLayout();

    FCSCaptionStyle style = FCSCaptionStyle();
    style.FontInfo = layout.FontInfo;
    style.FontInfo.TypefaceFontName = settings->CaptionTypeface;
    style.BackColour = settings->ColourProfile.LoadSynchronous()->CaptionBackColour;
    style.TextColour = settings->GetCaptionTextColour(source);
    style.TextPadding = layout.TextPadding;

    return style;
}
#endif //WITH_EDITOR