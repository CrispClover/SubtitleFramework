// Copyright Crisp Clover.

#include "CSToolsBFL.h"
#include "CSLanguageData.h"
#include "Overlay/Public/BasicOverlays.h"

float UCSToolsBFL::CalculateDisplayTime(const int32 unitCount, UCSLanguageData const* languageData, const bool unitIsWords, const float minSubTime)
{
	float unitTime;

	if (unitIsWords)
		unitTime = languageData->WordTime;
	else
		unitTime = languageData->CharacterTime;

	return FMath::Max(minSubTime, unitCount * unitTime);
}

TArray<FString> UCSToolsBFL::AutomaticLineBreaks(FString const& subtitle, int32& wordCount, const int32 maxLineLength, FString const& wordDelimiter)
{
	TArray<FString> words;
	subtitle.ParseIntoArray(words, wordDelimiter.GetCharArray().GetData());

	wordCount = words.Num();

	TArray<FString> lines;
	FString line;
	int32 LineLength = 0;

	for (FString word : words)
	{
		int32 temp = word.Len() + LineLength;//potential line length

		if (temp == maxLineLength) //append word to line, add line to lines, start new empty line
		{
			lines.Add(line + word);
			line = "";
			LineLength = 0;
		}
		else if (temp < maxLineLength) {//append delimiter to word, append word to line 
			line.Append(word + wordDelimiter);
			LineLength = temp + 1;
		}
		else //add line to lines, start new line with word and delimiter
		{
			lines.Add(line);
			line = word + wordDelimiter;
			LineLength = word.Len() + 1;
		}
	}

	if (LineLength != 0) //add (possibly) remaining line to lines
		lines.Add(line);

	return lines;
}

int32 UCSToolsBFL::CountWords(TArray<FString> const& lines, TArray<FString> const& wordCountDelimiters, int32 wordCount, FString const& wordDelimiter)
{
	if (wordCount == 0)//Performs full recount of words. E.g. for subtitles with custom line breaks.
	{
		for (FString line : lines)
		{
			TArray<FString> words;
			line.ParseIntoArray(words, wordDelimiter.GetCharArray().GetData());

			wordCount += words.Num();
		}
	}

	for (FString line : lines)
	{
		TArray<FString> wordCuts;

		for (FString delimiter : wordCountDelimiters)
		{
			line.ParseIntoArray(wordCuts, delimiter.GetCharArray().GetData());
			wordCount += wordCuts.Num() - 1;//Only count new words.
		}
	}

	return wordCount;
}

int32 UCSToolsBFL::CountCharacters(TArray<FString> const& lines, FString const& nonContributingCharacters)
{
	int32 charCount = 0;

	for (FString line : lines)
	{
		charCount += line.Len();

		for (TCHAR character : line)
		{
			for (TCHAR ncc : nonContributingCharacters)
			{
				if (character == ncc)
				{
					charCount--;
					break;
				}
			}
		}
	}

	return charCount;
}

TArray<FRawSubtitle> UCSToolsBFL::ConvertSRT(TSoftObjectPtr<UBasicOverlays> overlays)
{
	UBasicOverlays* oLoaded = overlays.LoadSynchronous();
	TArray<FRawSubtitle> result;

	for (FOverlayItem oItem : oLoaded->Overlays)
	{
		FRawSubtitle subtitle;
		subtitle.StartDelay = oItem.StartTime.GetTotalSeconds();
		subtitle.ReadDuration = oItem.EndTime.GetTotalSeconds() - subtitle.StartDelay;

		TArray<FString> strings;
		TArray<FText> lines;
		oItem.Text.ParseIntoArrayLines(strings);

		for (FString string : strings)
			lines.Add(FText::FromString(string));

		subtitle.Lines = lines;
		//TODO: Description?
		result.Add(subtitle);
	}

	return result;
}

TArray<FSoundCaption> UCSToolsBFL::SortCapByStartTime(TArray<FSoundCaption> const& captions)
{
	return SortByStartTime(captions);
}

TArray<FRawSubtitle> UCSToolsBFL::SortRawByStartTime(TArray<FRawSubtitle> const& subtitles)
{
	return SortByStartTime(subtitles);
}

TArray<FGroupSubtitle> UCSToolsBFL::SortGroupByStartTime(TArray<FGroupSubtitle> const& subtitles)
{
	return SortByStartTime(subtitles);
}

template <>
TArray<FSoundCaption> UCSToolsBFL::SortByStartTime<FSoundCaption>(TArray<FSoundCaption> const& captions)
{
	TArray<FSoundCaption> sorted;
	sorted.Reserve(captions.Num());

	for (int32 i = 0; i < captions.Num(); i++)
	{
		const FSoundCaption cap = captions[i];
		bool inserted = false;

		for (int32 n = 0; n < sorted.Num(); n++)
		{
			if (sorted[n].StartDelay > cap.StartDelay)
			{
				sorted.Insert(cap, n);
				inserted = true;
				break;
			}
		}

		if (!inserted)
			sorted.Add(cap);
	}

	return sorted;
}