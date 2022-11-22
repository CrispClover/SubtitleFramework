// Copyright Crisp Clover.

#include "CSUserSettings.h"

void UCSUserSettings::RecalculateLayout(UGameViewportClient const* viewportClient)
{
	if (!viewportClient)
		return;

	FVector2D viewportSize;
	viewportClient->GetViewportSize(viewportSize);
	const int32 size = FMath::Min(viewportSize.X, viewportSize.Y);

	if (iCachedLayout.Size == size)
		return;

	iCachedLayout.Size = size;
	iRecalculateLayout();
}

void UCSUserSettings::RecalculateDesignLayout(const FIntPoint screenSize)
{
	const int32 size = FMath::Min(screenSize.X, screenSize.Y);

	if (iCachedLayout.Size == size)
		return;

	iCachedLayout.Size = size;
	iRecalculateLayout();
}

void UCSUserSettings::iRecalculateLayout()
{
	iCachedLayout.FontInfo.FontObject = Font.LoadSynchronous();
	iCachedLayout.FontInfo.OutlineSettings = Outline;
	iCachedLayout.FontInfo.LetterSpacing = LetterSpacing;
	iCachedLayout.FontInfo.FontMaterial = FontMaterial;
	iCachedLayout.FontInfo.Size = iCachedLayout.Size * TextSize;

	iCachedLayout.TextPadding = FMargin(iCachedLayout.FontInfo.Size * .5f, iCachedLayout.FontInfo.Size * .25f);
	iCachedLayout.LinePadding = FMargin(0, iCachedLayout.Size * LinePadding, 0, 0);
	iCachedLayout.SubtitlePadding = FMargin(0, iCachedLayout.Size * SubtitlePadding, 0, 0);
	iCachedLayout.BoxPadding = FMargin(iCachedLayout.Size * .03f, iCachedLayout.Size * .01f);
	iCachedLayout.CaptionPadding = FMargin(0, iCachedLayout.Size * CaptionPadding, 0, 0);

	iCachedLayout.IndicatorSize = FVector2D(iCachedLayout.Size * IndicatorSize);
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

FCSLineStyle UCSUserSettings::GetLineStyle(FName speaker)
{
	FCSLineStyle lineStyle = FCSLineStyle();

	lineStyle.FontInfo = iCachedLayout.FontInfo;
	lineStyle.TextColour = GetTextColour(speaker);//TODO
	lineStyle.TextPadding = iCachedLayout.TextPadding;
	lineStyle.LineBackColour = LineBackColour;

	return lineStyle;
}
