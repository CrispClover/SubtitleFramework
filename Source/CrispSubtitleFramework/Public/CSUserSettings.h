// Copyright Crisp Clover.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CSUserSettings.generated.h"

class UCSLineWidget;
class UCSLetterboxWidget;
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
		int32 Size;

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
};

//The data used to style a subtitle line
USTRUCT(BlueprintType)
struct FCSLineStyle
{
	GENERATED_BODY()
		
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CrispSubtitles")
		FSlateFontInfo FontInfo;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CrispSubtitles")
		FLinearColor TextColour;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CrispSubtitles")
		FMargin TextPadding;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CrispSubtitles")
		FLinearColor LineBackColour;
};

/**
 * 
 */
UCLASS()
class CRISPSUBTITLEFRAMEWORK_API UCSUserSettings : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	/** TODO
	 * The name used to compare settings when calculating layout data.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CrispSubtitles")
		FName ID = FName("constructed");

	//The name to show to a user.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CrispSubtitles")
		FText DisplayName = FText::FromString("Default");

	//Whether to show subtitles.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Subtitles")
		bool bShowSubtitles = true;

	//Whether to show direction indicators on subtitles.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Subtitles")
		bool bShowSubtitleIndicators = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Subtitles|Widgets")
		TSoftClassPtr<UCSLetterboxWidget> LetterboxClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Subtitles|Widgets")
		TSoftClassPtr<UCSLineWidget> LineClass;

	//Whether to show captions.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Captions")
		bool bShowCaptions = true;

	//Whether to show direction indicators on captions.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Captions")
		bool bShowCaptionIndicators = true;//TODO

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Captions|Widgets")
		TSoftClassPtr<UCSCaptionWidget> CaptionClass;

#pragma region LABEL
public:
	//TODO
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Subtitles|Label")
		FText FullLabelFormat = FText::FromString("{speaker}: [{description}]");

	//TODO
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Subtitles|Label")
		FText SpeakerOnlyLabelFormat = FText::FromString("{speaker}:");

	//TODO
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Subtitles|Label")
		FText DescriptionOnlyLabelFormat = FText::FromString("[{description}]");

	//Whether to show the speaker's name.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Subtitles|Label")
		EShowSpeakerType ShowSpeaker = EShowSpeakerType::Always;

	//Whether to convert the speaker's name to uppercase letters.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Subtitles|Label")
		bool bSpeakersAreUpperCase = true;

	/**
	 * Whether to show any potential additional information included in the subtitle.
	 * This is intended for narration or speech heard over radio for example. TODO
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Subtitles|Label")
		bool bShowSubtitleDescriptions = false;

#pragma endregion
	
#pragma region STYLE
public:
	//The padding in between lines. (in screen %)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Subtitles|Layout")
		float LinePadding = 0.f;

	/**
	 * The text size should fit into a line height of 8% screen height (recommended for TV).
	 * Smaller sizes will be a better fit for most games.
	 * The default (4%) fits a 6% line height with the Roboto font. You might have to adjust this value when using other fonts.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Layout")
		float TextSize = .04f;

	//The size of indicators relative to the text size
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Layout")
		float IndicatorSize = .1f;

	//The padding between individual subtitles. (in screen %)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Subtitles|Layout")
		float SubtitlePadding = .015f;

	//The padding in between captions. (in screen %)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Captions|Layout")
		float CaptionPadding = 0.01f;

	//Which rules to follow when colour coding the subtitles.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CrispSubtitles")
		EColourCodeType ColourCoding = EColourCodeType::TODO;

	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Style")
		FLinearColor DefaultTextColour = FLinearColor::White;

	//This Map matches speakers to their text colours. This can be done dynamically (see: AvailableTextColours & TODO: function for matching)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Font", meta = (AllowedClasses = "Font"))
		TMap<FName, FLinearColor> AssignedTextColours = TMap<FName, FLinearColor>();

	/**
	 * These colours can be assigned dynamically, based on priority. The last colour will always be used for all characters without assigned colours.
	 * If you wish to assign colours before runtime, use "AssignedTextColours". (e.g. when the number of characters in your game is limited, 
	 * or if you want an easy way to enforce main characters always using the same colours across gameplay sessions.)
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Font", meta = (AllowedClasses = "Font"))
		TArray<FLinearColor> AvailableTextColours = TArray<FLinearColor>
	{
		  FLinearColor::White
		, FLinearColor::Yellow
		, FLinearColor(0,1,1)
		, FLinearColor::Green
	};

	//The colour of the letterbox, should have a high contrast compared to the text colours.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Style")
		FLinearColor LetterboxColour = FLinearColor::FLinearColor(0, 0, 0, .5f);

	//The colour of the background behind the individual lines, should have a high contrast compared to the text colours.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Style")
		FLinearColor LineBackColour = FLinearColor::Black;

	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Style")
		FLinearColor CaptionBackColour;

#pragma endregion
	
#pragma region FONT
public:
	//The font to use.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Font", meta = (AllowedClasses = "Font"))
		TSoftObjectPtr<const UObject> Font;

	//The settings for the font outline.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Font")
		FFontOutlineSettings Outline = FFontOutlineSettings();

	//The uniform spacing (or tracking) between all characters in the text.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Font", meta = (ClampMin = -1000, ClampMax = 10000))
		int32 LetterSpacing = 0;

	//The material to use when rendering this font, can be left blank.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Font", meta = (AllowedClasses = "MaterialInterface"))
		TObjectPtr<UObject> FontMaterial;

#pragma endregion
	
#pragma region TIME
public:
	//The time gap (in seconds) between removing/adding subtitles to reduce flickering.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timing")
		float TimeGap = .16f;

	//The minimum time (in seconds) any subtitle is on screen.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timing")
		float MinimumSubtitleTime = 1.f;

	//The minimum time (in seconds) any caption is on screen.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timing")
		float MinimumCaptionTime = .85f;

	//Slow/fast readers could use this to adjust the time subtitles are displayed when you expose this to the user settings.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timing")
		float ReadingSpeed = 1.f;
	
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

	//TODO
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Style")
		FCSLineStyle GetLineStyle(FName Speaker);//TODO: custom logic

#pragma endregion

private:
	void iRecalculateLayout();

	FLayoutCacheData iCachedLayout;
};