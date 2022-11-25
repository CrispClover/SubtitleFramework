// Copyright Crisp Clover.

#include "CSUILibrary.h"
#include "CSUserSettings.h"
#include "CSProjectSettingFunctions.h"

FCSLineStyle::FCSLineStyle()
    : FontInfo()//TODO: fallback needed?
    , TextColour(FLinearColor::White)
    , BackColour(FLinearColor::Black)
    , TextPadding()
{
	static const FName SpecialName_DefaultSystemFont("DefaultSystemFont");
    FSlateFontInfo fontInfo = FSlateFontInfo();

};

FCSLetterboxStyle::FCSLetterboxStyle()
    : LineClass(UCSProjectSettingFunctions::GetDefaultSettings()->LineClass.LoadSynchronous())
    , LineStyle()
    , LetterboxColour(FLinearColor::Black)
    , BoxPadding()
    , LinePadding()
    , bShowIndicator(true)
{};

FCSCaptionStyle::FCSCaptionStyle()
    : FontInfo()//TODO: fallback needed?
    , TextColour(FLinearColor::White)
    , BackColour(FLinearColor::Black)
    , TextPadding()
    , bShowIndicator(true)
{};

FCSLetterboxStyle UCSUILibrary::GetLetterboxStyle(UCSUserSettings* settings, const FName speaker)
{
    if (!settings)
        return FCSLetterboxStyle();

    FLayoutCacheData const& layout = settings->GetLayout();

    FCSLetterboxStyle style = FCSLetterboxStyle();

    style.LetterboxColour = settings->LetterboxColour;
    style.LineClass = settings->LineClass.LoadSynchronous();
    style.LineStyle = GetLineStyle(settings, speaker);
    style.BoxPadding = layout.BoxPadding;
    style.LinePadding = layout.LinePadding;
    style.bShowIndicator = settings->bShowSubtitleIndicators;

    return style;
}

FCSLineStyle UCSUILibrary::GetLineStyle(UCSUserSettings* settings, const FName speaker)
{
    if (!settings)
        return FCSLineStyle();

    FLayoutCacheData const& layout = settings->GetLayout();

    FCSLineStyle style = FCSLineStyle();
    style.FontInfo = layout.FontInfo;
    style.BackColour = settings->LineBackColour;
    style.TextColour = settings->GetTextColour(speaker);
    style.TextPadding = layout.TextPadding;

    return style;
}

FCSCaptionStyle UCSUILibrary::GetCaptionStyle(UCSUserSettings* settings, const FName source)
{
    if (!settings)
        return FCSCaptionStyle();

    FLayoutCacheData const& layout = settings->GetLayout();

    FCSCaptionStyle style = FCSCaptionStyle();
    style.FontInfo = layout.FontInfo;
    style.FontInfo.Size = layout.CaptionTextSize;
    style.FontInfo.TypefaceFontName = settings->CaptionTypeface;
    style.BackColour = settings->CaptionBackColour;
    style.TextColour = settings->GetTextColour(source);//TODO: caption colouring mode
    style.TextPadding = layout.TextPadding;

    return style;
}

FCSLetterboxStyle UCSUILibrary::GetDesignLetterboxStyle(const FName speaker)
{
    UCSUserSettings* settings = UCSProjectSettingFunctions::GetDesignSettings();

    if (!settings)
        return FCSLetterboxStyle();
    
    FLayoutCacheData const& layout = settings->GetLayout();

    FCSLetterboxStyle style = FCSLetterboxStyle();
    style.LetterboxColour = settings->LetterboxColour;
    style.LineClass = settings->LineClass.LoadSynchronous();
    style.BoxPadding = layout.BoxPadding;
    style.LinePadding = layout.LinePadding;
    style.LineStyle = GetDesignLineStyle(speaker);
    style.bShowIndicator = settings->bShowSubtitleIndicators;

    return style;
}

FCSLineStyle UCSUILibrary::GetDesignLineStyle(const FName speaker)
{
    UCSUserSettings* settings = UCSProjectSettingFunctions::GetDesignSettings();

    if (!settings)
        return FCSLineStyle();
    
    FLayoutCacheData const& layout = settings->GetLayout();

    FCSLineStyle style = FCSLineStyle();
    style.FontInfo = layout.FontInfo;
    style.BackColour = settings->LineBackColour;
    style.TextColour = settings->GetTextColour(speaker);
    style.TextPadding = layout.TextPadding;

    return style;
}

FCSCaptionStyle UCSUILibrary::GetDesignCaptionStyle(const FName source)
{
    UCSUserSettings* settings = UCSProjectSettingFunctions::GetDesignSettings();

    if (!settings)
        return FCSCaptionStyle();

    FLayoutCacheData const& layout = settings->GetLayout();

    FCSCaptionStyle style = FCSCaptionStyle();
    style.FontInfo = layout.FontInfo;//TODO
    style.BackColour = settings->CaptionBackColour;
    style.TextColour = settings->GetTextColour(source);//TODO: caption colouring mode
    style.TextPadding = layout.TextPadding;

    return style;
}