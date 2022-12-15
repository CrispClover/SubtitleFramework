// Copyright Crisp Clover.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CSUserSettings.generated.h"

class UCSLetterboxWidget;
class UCSLineWidget;
class UCSCaptionWidget;
class UCSBaseSpacer;
class UUserWidget;
class UCSColourProfile;

UENUM(BlueprintType)
enum class EShowSpeaker : uint8
{
	Always,
	ColourCodedShowOnce,
	ColourCodedShowNever,
	Never
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
		float BaseSize;
};

/** TODO?
* meta=(DisplayAfter="abc")
* meta=(DisplayPriority=123)
*/

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
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bShowSubtitles", EditConditionHides, NoResetToDefault), Category = "Subtitles")
		TSoftClassPtr<UCSLetterboxWidget> LetterboxClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bShowSubtitles", EditConditionHides, NoResetToDefault), Category = "Subtitles")
		TSoftClassPtr<UCSBaseSpacer> SubtitleSpacer;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "bShowSubtitles", EditConditionHides, NoResetToDefault), Category = "Subtitles")
		TSoftClassPtr<UCSLineWidget> LineClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bShowCaptions", EditConditionHides, NoResetToDefault), Category = "Captions")
		TSoftClassPtr<UCSCaptionWidget> CaptionClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bShowSubtitles", EditConditionHides, NoResetToDefault), Category = "Subtitles")
		TSoftClassPtr<UCSBaseSpacer> CaptionSpacer;

	//Whether to show subtitles.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Subtitles")
		bool bShowSubtitles;

	//Whether to show direction indicators on subtitles.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bShowSubtitles", EditConditionHides), Category = "Subtitles")
		bool bShowSubtitleIndicators;

	//Whether to show captions.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Captions")
		bool bShowCaptions;

	//Whether to show direction indicators on captions.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bShowCaptions", EditConditionHides), Category = "Captions")
		bool bShowCaptionIndicators;

	//Determines how colours are applied to the UI.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bShowSubtitles||bShowCaptions", EditConditionHides), Category = "Colours")
		TSoftObjectPtr<UCSColourProfile> ColourProfile;
	
#pragma endregion

#pragma region LABEL
public:
	//The text format when speaker and description are both shown.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bShowSubtitles", EditConditionHides), Category = "SubtitleLabel")
		FText FullLabelFormat;

	//The text format when only the speaker is shown.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bShowSubtitles", EditConditionHides), Category = "SubtitleLabel")
		FText SpeakerOnlyLabelFormat;

	//The text format when only the description is shown.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bShowSubtitles", EditConditionHides), Category = "SubtitleLabel")
		FText DescriptionOnlyLabelFormat;

	//Whether to show the speaker's name.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bShowSubtitles", EditConditionHides), Category = "SubtitleLabel")
		EShowSpeaker ShowSpeaker;

	//Whether to convert the speaker's name to uppercase letters.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bShowSubtitles", EditConditionHides), Category = "SubtitleLabel")
		bool bSpeakersAreUpperCase;

	//Whether to show the descriptive information included in the subtitles.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bShowSubtitles", EditConditionHides), Category = "SubtitleLabel")
		bool bShowSubtitleDescriptions;

#pragma endregion
	
#pragma region FONT
public:
	//The font to use.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bShowSubtitles||bShowCaptions", EditConditionHides, AllowedClasses = "Font"), Category = "Font")
		TSoftObjectPtr<const UObject> Font;
	
	//The typeface to use for subtitles.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bShowSubtitles", EditConditionHides, GetOptions = "GetTypefaceOptions"), Category = "Font")
		FName RegularTypeface;
	
	//The typeface to use when a subtitle's source and speaker mismatch.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bShowSubtitles", EditConditionHides, GetOptions = "GetTypefaceOptions"), Category = "Font")
		FName IndirectSpeechTypeface;

	//The typeface to use for captions.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bShowCaptions", EditConditionHides, GetOptions = "GetTypefaceOptions"), Category = "Font")
		FName CaptionTypeface;

	//The settings for the font outline.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Font")
		FFontOutlineSettings Outline;

	//The uniform spacing (or tracking) between all characters in the text.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, meta = (UIMin = -1000, UIMax = 10000), Category = "Font")
		int32 LetterSpacing;

	//The material to use when rendering this font, can be left blank.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, meta = (AllowedClasses = "MaterialInterface"), Category = "Font")
		TObjectPtr<UObject> FontMaterial;

	/**
	 * The text size should fit into a line height of 8% screen height (recommended for TV).
	 * Smaller sizes will be a better fit for most games.
	 * The default text size (4%) fits a 6% line height with the Roboto font. You might have to adjust this value when using other fonts.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bShowSubtitles", EditConditionHides), Category = "Font")
		float SubtitleTextSize;

	//The text size of a caption. (In screen %)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bShowCaptions", EditConditionHides), Category = "Font")
		float CaptionTextSize;

#pragma endregion

#pragma region LAYOUT
public:
	//The padding between individual subtitles. (In screen %)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bShowSubtitles", EditConditionHides), Category = "Layout")
		float SubtitlePadding;

	//The padding in between lines. (In screen %)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bShowSubtitles", EditConditionHides), Category = "Layout")
		float LinePadding;

	//The padding in between captions. (In screen %)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bShowCaptions", EditConditionHides), Category = "Layout")
		float CaptionPadding;

	//The horizontal alignment of each caption inside the container.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bShowCaptions", EditConditionHides), Category = "Layout")
		TEnumAsByte<EHorizontalAlignment> CaptionAlignment;

	//The size of indicators. (relative to the text size)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bShowSubtitleIndicators||bShowCaptionIndicators", EditConditionHides), Category = "Layout")
		float IndicatorSize;

#pragma endregion
	
#pragma region TIMING
public:
	/**
	 * Slow/fast readers could use this to adjust the time for which subtitles are displayed when you expose this to the user settings.
	 * Negative numbers cause all subtitles to be displayed indefinitely. @see TODO: link
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (UIMin = 1.f, UIMax = 2.f, EditCondition = "bShowSubtitles", EditConditionHides), Category = "Timing")
		float ReadingSpeed;

	//The time gap (in seconds) between removing/adding subtitles to reduce flickering. If you ever change this, please only increase the value.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, meta = (UIMin = 0.16f, UIMax = 0.5f, EditCondition = "bShowSubtitles||bShowCaptions", EditConditionHides), Category = "Timing")
		float TimeGap;

	//The minimum time (in seconds) any subtitle is on screen. If you ever change this, please only increase the value.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, meta = (UIMin = 1.f, UIMax = 1.5f, EditCondition = "bShowSubtitles", EditConditionHides), Category = "Timing")
		float MinimumSubtitleTime;

	//The minimum time (in seconds) any caption is on screen. If you ever change this, please only increase the value.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, meta = (UIMin = 0.85f, UIMax = 1.f, EditCondition = "bShowCaptions", EditConditionHides), Category = "Timing")
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
		void RecalculateDesignLayout(FVector2D const& ScreenSize);

	//Returns whether a subtitle should show the speaker's display-name.
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Label")
		bool GetShowSpeaker(const FName Speaker) const;

	//Logs when a speaker's display-name was shown to the user.
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Label")
		void LogSpeakerShown(const FName Speaker) const;
	
	//Returns the text colour for subtitles as defined by ColourProfile.
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Style")
		FLinearColor const& GetSubtitleTextColour(const FName Speaker) const;
	
	//Returns the text colour for captions as defined by ColourProfile.
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Style")
		FLinearColor const& GetCaptionTextColour(const FName Speaker) const;
	
private:
	void iRecalculateLayout();

	FLayoutCacheData iCachedLayout = FLayoutCacheData();

#pragma endregion

#if WITH_EDITOR
private:
	UFUNCTION()
		TArray<FName> GetTypefaceOptions() const;
#endif
};