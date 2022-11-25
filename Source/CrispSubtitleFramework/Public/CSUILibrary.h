// Copyright Crisp Clover.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CSUILibrary.generated.h"

class UCSUserSettings;
class UCSLineWidget;

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
		  TSubclassOf<UCSLineWidget> lineClass, FCSLineStyle lineStyle, FLinearColor const& boxColour
		, FMargin const& boxPadding, FMargin const& linePadding, const bool showIndicator
	)
		: LineClass(lineClass)
		, LineStyle(lineStyle)
		, LetterboxColour(boxColour)
		, BoxPadding(boxPadding)
		, LinePadding(linePadding)
		, bShowIndicator(showIndicator)
	{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CrispSubtitles")
		TSubclassOf<UCSLineWidget> LineClass;

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

/**
 * 
 */
UCLASS()
class CRISPSUBTITLEFRAMEWORK_API UCSUILibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|UI")
		static FCSLetterboxStyle GetLetterboxStyle(UCSUserSettings* UserSettings, const FName Speaker);
	
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|UI")
		static FCSLineStyle GetLineStyle(UCSUserSettings* UserSettings, const FName Speaker);

	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|UI")
		static FCSCaptionStyle GetCaptionStyle(UCSUserSettings* UserSettings, const FName Source);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "DesignLetterboxStyle", CompactNodeTitle = "->", BlueprintAutocast), Category = "CrispSubtitles|UI")
		static FCSLetterboxStyle GetDesignLetterboxStyle(const FName Speaker);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "DesignLineStyle", CompactNodeTitle = "->", BlueprintAutocast), Category = "CrispSubtitles|UI")
		static FCSLineStyle GetDesignLineStyle(const FName Speaker);
	
	UFUNCTION(BlueprintPure, meta = (DisplayName = "DesignCaptionStyle", CompactNodeTitle = "->", BlueprintAutocast), Category = "CrispSubtitles|UI")
		static FCSCaptionStyle GetDesignCaptionStyle(const FName Source);
};
