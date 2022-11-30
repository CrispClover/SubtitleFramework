// Copyright Crisp Clover. Feel free to copy.

#include "CSColourProfile.h"

FLinearColor const& UCSCPSimple::oGetColour(FName speaker) const
{
	return DefaultColour;
}

FLinearColor const& UCSCPAssigned::oGetColour(FName speaker) const
{
	if (FLinearColor const* colour = AssignedColours.Find(speaker))
		return *colour;
	else
		return DefaultColour;
}

bool UCSCPAssigned::oColourWasMatched(FName speaker) const
{
	return oMatchedSpeakers.Contains(speaker);
}
	
void UCSCPAssigned::oLogMatch(FName speaker)
{
	if (AssignedColours.Contains(speaker))
		oMatchedSpeakers.Add(speaker);
}

void UCSCPAssigned::oForgetMatch(FName speaker)
{
	oMatchedSpeakers.Remove(speaker);
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