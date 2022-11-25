// Copyright Crisp Clover.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CSUserSettings.generated.h"

class UCSLetterboxWidget;
class UCSLineWidget;
class UCSCaptionWidget;

UENUM(BlueprintType)
enum class EShowSpeakerType : uint8
{
	Always,
	ColourCoded,
	Never
};

UENUM(BlueprintType)
enum class EColourCodeType : uint8
{
	Assigned,//TODO!
	TODO,
	None,
	Custom
};

//The data used to cache the layout of the subtitles.
USTRUCT(BlueprintType)
struct FLayoutCacheData
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CrispSubtitles")
		FSlateFontInfo FontInfo = FSlateFontInfo();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CrispSubtitles")
		FMargin TextPadding = FMargin();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CrispSubtitles")
		FMargin LinePadding = FMargin();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CrispSubtitles")
		FMargin SubtitlePadding = FMargin();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CrispSubtitles")
		FMargin BoxPadding = FMargin();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CrispSubtitles")
		FMargin CaptionPadding = FMargin();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CrispSubtitles")
		FVector2D IndicatorSize = FVector2D();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CrispSubtitles")
		int32 CaptionTextSize;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CrispSubtitles")
		int32 BaseSize;
};

/**
 * 
 */
UCLASS()
class CRISPSUBTITLEFRAMEWORK_API UCSUserSettings : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UCSUserSettings();
	
#pragma region CORE
public:
	//The name to show to a user.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CrispSubtitles")
		FText DisplayName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
		TSoftClassPtr<UCSLetterboxWidget> LetterboxClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Widgets")
		TSoftClassPtr<UCSLineWidget> LineClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
		TSoftClassPtr<UCSCaptionWidget> CaptionClass;

	//Whether to show subtitles.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Subtitles")
		bool bShowSubtitles;

	//Whether to show direction indicators on subtitles.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Subtitles")
		bool bShowSubtitleIndicators;

	//Whether to show captions.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Captions")
		bool bShowCaptions;

	//Whether to show direction indicators on captions.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Captions")
		bool bShowCaptionIndicators;
	
#pragma endregion

#pragma region LABEL
public:
	//The text format when speaker and description are both shown.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SubtitleLabel")
		FText FullLabelFormat;

	//The text format when only the speaker is shown.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SubtitleLabel")
		FText SpeakerOnlyLabelFormat;

	//The text format when only the description is shown.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SubtitleLabel")
		FText DescriptionOnlyLabelFormat;

	//Whether to show the speaker's name.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SubtitleLabel")
		EShowSpeakerType ShowSpeaker;

	//Whether to convert the speaker's name to uppercase letters.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SubtitleLabel")
		bool bSpeakersAreUpperCase;

	/**
	 * Whether to show any potential additional information included in the subtitle.
	 * This is intended for narration or speech heard over radio for example. TODO
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SubtitleLabel")
		bool bShowSubtitleDescriptions;

#pragma endregion
	
#pragma region FONT
public:
	//The font to use.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Font", meta = (AllowedClasses = "Font"))
		TSoftObjectPtr<const UObject> Font;
	
	//The typeface to use for subtitles.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Font", meta = (GetOptions = "GetTypefaceOptions"))
		FName RegularTypeface;
	
	//The typeface to use when a subtitle's source and speaker mismatch.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Font", meta = (GetOptions = "GetTypefaceOptions"))
		FName ItalicTypeface;

	//The typeface to use for captions.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Font", meta = (GetOptions = "GetTypefaceOptions"))
		FName CaptionTypeface;

	//The settings for the font outline.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Font")
		FFontOutlineSettings Outline;

	//The uniform spacing (or tracking) between all characters in the text.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Font", meta = (ClampMin = -1000, ClampMax = 10000))
		int32 LetterSpacing;

	//The material to use when rendering this font, can be left blank.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Font", meta = (AllowedClasses = "MaterialInterface"))
		TObjectPtr<UObject> FontMaterial;

	/**
	 * The text size should fit into a line height of 8% screen height (recommended for TV).
	 * Smaller sizes will be a better fit for most games.
	 * The default text size (4%) fits a 6% line height with the Roboto font. You might have to adjust this value when using other fonts.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Font")
		float SubtitleTextSize;

	//TODO
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Font")
		float CaptionTextSize;

	UFUNCTION()
		TArray<FName> GetTypefaceOptions() const;

#pragma endregion
		
#pragma region COLOURS
public:
	//Which rules to follow when colour coding the subtitles.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Colours")
		EColourCodeType ColourCoding;

	//The default text colour.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Colours")
		FLinearColor DefaultTextColour;

	//This Map matches speakers to their text colours. This can be done dynamically (see: AvailableTextColours & TODO: function for matching)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Colours")
		TMap<FName, FLinearColor> AssignedTextColours;

	/**
	 * These colours can be assigned dynamically, based on priority. The last colour will always be used for all characters without assigned colours.
	 * If you wish to assign colours before runtime, use "AssignedTextColours". (e.g. when the number of characters in your game is limited, 
	 * or if you want an easy way to enforce main characters always using the same colours across gameplay sessions.)
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Colours")
		TArray<FLinearColor> AvailableTextColours;

	//The colour of the letterbox, should have a high contrast compared to the text colours.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Colours")
		FLinearColor LetterboxColour;

	//The colour of the background behind the individual lines, should have a high contrast compared to the text colours.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Colours")
		FLinearColor LineBackColour;

	//The colour of the background behind the captions, should have a high contrast compared to the text colours.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Colours")
		FLinearColor CaptionBackColour;

#pragma endregion

#pragma region LAYOUT
public:
	//The padding between individual subtitles. (in screen %)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Layout")
		float SubtitlePadding;

	//The padding in between lines. (in screen %)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Layout")
		float LinePadding;

	//The padding in between captions. (in screen %)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Layout")
		float CaptionPadding;

	//The size of indicators. (relative to the text size)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Layout")
		float IndicatorSize;

#pragma endregion
	
#pragma region TIMING
public:
	//Slow/fast readers could use this to adjust the time subtitles are displayed when you expose this to the user settings.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timing")
		float ReadingSpeed;

	//The time gap (in seconds) between removing/adding subtitles to reduce flickering. If you ever change this, please only increase the value.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timing")
		float TimeGap;

	//The minimum time (in seconds) any subtitle is on screen. If you ever change this, please only increase the value.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timing")
		float MinimumSubtitleTime;

	//The minimum time (in seconds) any caption is on screen. If you ever change this, please only increase the value.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timing")
		float MinimumCaptionTime;

#pragma endregion

#pragma region FUNCTIONS
public:
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Layout")
		FORCEINLINE FLayoutCacheData const& GetLayout() const
			{ return iCachedLayout; };
	
	//Recalculates the cached layout data if the smaller viewport dimension has changed.
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Layout")
		void RecalculateLayout(UGameViewportClient const* viewportClient);
	
	//Recalculates the cached layout data if the smaller dimension has changed.
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Layout")
		void RecalculateDesignLayout(const FIntPoint ScreenSize);

	//TODO
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Style")
		bool GetShowSpeaker(const FName speakerID) const;
	
	//TODO
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Style")
		FLinearColor const& GetTextColour(FName Speaker) const;//TODO: custom logic

private:
	void iRecalculateLayout();

	FLayoutCacheData iCachedLayout = FLayoutCacheData();

#pragma endregion
};