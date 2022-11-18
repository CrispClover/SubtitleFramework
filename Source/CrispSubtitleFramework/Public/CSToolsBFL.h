// Copyright Crisp Clover.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CSLanguageData.h"
#include "CSS_SubtitleGISS.h"
#include "Overlay/Public/BasicOverlays.h"
#include "CSToolsBFL.generated.h"

/*
 * 
 */
UCLASS()
class CRISPSUBTITLEFRAMEWORK_API UCSToolsBFL : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/*
	 * Estimates the reading time required for the subtitle based on word/character count and average reading speeds in a select few languages.
	 * @param UnitCount The number of words or characters
	 * @param LanguageData The data asset that contains the read time data for the subtitle's language.
	 * @param UnitIsWords true for words, false for characters
	 * @param MinSubtitleDuration The minimum duration for a subtitle. Default is 0.833s. (Complies with Netflix's Timed Text Style Guides)
	 */
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitleTools")
		static float CalculateDisplayTime(const int32 UnitCount, UCSLanguageData const* LanguageData, const bool UnitIsWords = true, const float MinSubtitleDuration = .833f);

	/*
	 * Introduces line breaks based on number of characters in one line. Also performs a (cheap) count of words on the subtitle.
	 * @param Subtitle The subtitle to break into an array.
	 * @param The maximum number of characters in a line. Default is 38. (Complies with the BBC's Subtitle Guidelines)
	 * @param WordDelimiter The character in between words. Default is space (" "). Allows multi-character delimiters.
	 */
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitleTools")
		static TArray<FString> AutomaticLineBreaks(FString const& Subtitle, int32& WordCount, const int32 MaxLineLength = 38, FString const& WordDelimiter = TEXT(" "));

	/*
	 * Counts words based on multiple delimiters. Important for hyphenated words for example.
	 * @param SubtitleLines The subtitle lines to perform the count on.
	 * @param WordCountDelimiters The additional characters in between words (e.g. hyphens). Allows multi-character delimiters.
	 * @param InitialWordCount Can use the word count from "AutomaticLineBreaks". If left at 0, a full recount of words is performed. Use 0 with custom line breaks.
	 * @param WordDelimiter The character in between words. Default is space (" "). Allows multi-character delimiters.
	 */
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitleTools")
		static int32 CountWords(TArray<FString> const& SubtitleLines, TArray<FString> const& WordCountDelimiters, int32 InitialWordCount = 0, FString const& WordDelimiter = TEXT(" "));

	/*
	 * Counts characters, while excluding all provided characters. Useful for languages that do not have word delimiters. (e.g. Thai)
	 * @param SubtitleLines The subtitle lines to perform the count on.
	 * @param NonContributingCharacters The characters to exclude from the result.
	 */
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitleTools")
		static int32 CountCharacters(TArray<FString> const& SubtitleLines, FString const& NonContributingCharacters = TEXT(" -.,;:!?"));

	UFUNCTION(BlueprintCallable, Category = "CrispSubtitleTools")
		static TArray<FRawSubtitle> ConvertSRT(TSoftObjectPtr<UBasicOverlays> Overlays);

	/*
	 * Sorts an array of captions by StartDelay.
	 */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Sort Captions", CompactNodeTitle = "sort"), Category = "CrispSubtitleTools")
		static TArray<FSoundCaption> SortCapByStartTime(TArray<FSoundCaption> const& Captions);

	/*
	 * Sorts an array of raw subtitles by StartDelay.
	 */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Sort Raw Subtitles", CompactNodeTitle = "sort"), Category = "CrispSubtitleTools")
		static TArray<FRawSubtitle> SortRawByStartTime(TArray<FRawSubtitle> const& Subtitles);

	//Sorts an array of group subtitles by StartDelay.
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Sort Group Subtitles", CompactNodeTitle = "sort"), Category = "CrispSubtitleTools")
		static TArray<FGroupSubtitle> SortGroupByStartTime(TArray<FGroupSubtitle> const& Subtitles);

	//Sorting template definition; Do not use.
	template <typename T>
	static inline TArray<T> SortByStartTime(TArray<T> const& arr)
	{ return arr; };

	//Actual sorting template
	template <>
	static inline TArray<FSoundCaption> SortByStartTime<FSoundCaption>(TArray<FSoundCaption> const& captions)//TODO: remove inlining
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
};
