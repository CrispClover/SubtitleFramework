// Copyright Crisp Clover.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CSUILibrary.generated.h"

class UCSBaseSpacer;
class UCSLineWidget;
class UCSUserSettings;

#pragma region STRUCTS
//The data used to style a subtitle line
USTRUCT(BlueprintType)
struct FCSLineStyle
{
	GENERATED_BODY()
		
public:
	FCSLineStyle();

	FCSLineStyle(FSlateFontInfo const& fontInfo, FLinearColor const& textColour, FLinearColor const& backColour, FMargin const& textPadding)
		: FontInfo(fontInfo)
		, TextColour(textColour)
		, BackColour(backColour)
		, TextPadding(textPadding)
	{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CrispSubtitles")
		FSlateFontInfo FontInfo;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CrispSubtitles")
		FLinearColor TextColour;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CrispSubtitles")
		FLinearColor BackColour;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CrispSubtitles")
		FMargin TextPadding;
};

//The data used to style a subtitle letterbox
USTRUCT(BlueprintType)
struct FCSLetterboxStyle
{
	GENERATED_BODY()
		
public:
	FCSLetterboxStyle();

	FCSLetterboxStyle
	(
		  TSubclassOf<UCSLineWidget> lineClass, FCSLineStyle const& lineStyle, FCSLineStyle const& labelStyle
		, FLinearColor const& boxColour, FMargin const& boxPadding, FMargin const& linePadding, const bool showIndicator
	)
		: LineClass(lineClass)
		, LabelStyle(labelStyle)
		, LineStyle(lineStyle)
		, LetterboxColour(boxColour)
		, BoxPadding(boxPadding)
		, LinePadding(linePadding)
		, bShowIndicator(showIndicator)
	{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CrispSubtitles")
		TSubclassOf<UCSLineWidget> LineClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CrispSubtitles")
		FCSLineStyle LabelStyle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CrispSubtitles")
		FCSLineStyle LineStyle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CrispSubtitles")
		FLinearColor LetterboxColour;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CrispSubtitles")
		FMargin BoxPadding;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CrispSubtitles")
		FMargin LinePadding;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CrispSubtitles")
		bool bShowIndicator;
};

//The data used to style a caption
USTRUCT(BlueprintType)
struct FCSCaptionStyle
{
	GENERATED_BODY()
		
public:
	FCSCaptionStyle();

	FCSCaptionStyle(FSlateFontInfo const& fontInfo, FLinearColor const& textColour, FLinearColor const& backColour, FMargin const& textPadding, const bool showIndicator)
		: FontInfo(fontInfo)
		, TextColour(textColour)
		, BackColour(backColour)
		, TextPadding(textPadding)
		, bShowIndicator(showIndicator)
	{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CrispSubtitles")
		FSlateFontInfo FontInfo;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CrispSubtitles")
		FLinearColor TextColour;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CrispSubtitles")
		FLinearColor BackColour;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CrispSubtitles")
		FMargin TextPadding;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CrispSubtitles")
		bool bShowIndicator;
};
#pragma endregion

/**
 * 
 */
UCLASS()
class CRISPSUBTITLEFRAMEWORK_API UCSUILibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|UI")
		static FCSLetterboxStyle GetLetterboxStyle(UCSUserSettings const* UserSettings, const FName Speaker, const bool bIsIndirectSpeech);
	
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|UI")
		static FCSLineStyle GetLabelStyle(UCSUserSettings const* UserSettings, const FName Speaker);
	
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|UI")
		static FCSLineStyle GetLineStyle(UCSUserSettings const* UserSettings, const FName Speaker, const bool bIsIndirectSpeech);

	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|UI")
		static FCSCaptionStyle GetCaptionStyle(UCSUserSettings const* UserSettings, const FName Source);
	
#if WITH_EDITOR
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|UI")
		static FCSLetterboxStyle GetDesignLetterboxStyle(const FName Speaker, const bool bIsIndirectSpeech, FVector2D const& ScreenSize);

	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|UI")
		static FCSLineStyle GetDesignLabelStyle(const FName Speaker, FVector2D const& ScreenSize);

	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|UI")
		static FCSLineStyle GetDesignLineStyle(const FName Speaker, const bool bIsIndirectSpeech, FVector2D const& ScreenSize);
	
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|UI")
		static FCSCaptionStyle GetDesignCaptionStyle(const FName Source, FVector2D const& ScreenSize);
#endif
};
