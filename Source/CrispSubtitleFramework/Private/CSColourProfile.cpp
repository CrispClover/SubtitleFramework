// Copyright Crisp Clover.

#include "CSColourProfile.h"

FLinearColor const& UCSCPSimple::oGetSubtitleColour(FName speaker) const
{
	return DefaultSubtitleTextColour;
}

FLinearColor const& UCSCPSimple::oGetCaptionColour(FName source) const
{
	return DefaultCaptionTextColour;
}

bool UCSCPMatched::oColourWasMatched(FName speaker) const
{
	return oMatchedSpeakers.Contains(speaker);
}

void UCSCPMatched::oLogMatch(FName speaker)
{
	oMatchedSpeakers.Add(speaker);
}

void UCSCPMatched::oForgetMatch(FName speaker)
{
	oMatchedSpeakers.Remove(speaker);
}

bool UCSCPAssigned::oHasColour(FName speaker) const
{
	return AssignedColours.Contains(speaker);
}

FLinearColor const& UCSCPAssigned::oGetSubtitleColour(FName speaker) const
{
	if (FLinearColor const* colour = AssignedColours.Find(speaker))
	{
		return *colour;
	}
	else
	{
		return DefaultSubtitleTextColour;
	}
}

void UCSCPAssigned::oLogMatch(FName speaker)
{
	if (AssignedColours.Contains(speaker))
	{
		UCSCPMatched::oLogMatch(speaker);
	}
}

void UCSCPCustom::Empty()
{
	AssignedColours.Empty();
	oMatchedSpeakers.Empty();
}

void UCSCPCustom::Forget(FName speaker)
{
	AssignedColours.Remove(speaker);
	oMatchedSpeakers.Remove(speaker);
}

void UCSCPCustom::oAssignColour(FName speaker, FLinearColor colour)
{
	AssignedColours.FindOrAdd(speaker) = colour;
	oMatchedSpeakers.Remove(speaker);
}