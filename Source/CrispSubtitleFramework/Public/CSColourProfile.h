// Copyright Crisp Clover.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CSColourProfile.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class CRISPSUBTITLEFRAMEWORK_API UCSColourProfile : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UCSColourProfile()
		: LetterboxColour(FLinearColor::FLinearColor(0, 0, 0, .5f))
		, LineBackColour(FLinearColor::Transparent)
		, CaptionBackColour(FLinearColor::FLinearColor(0, 0, 0, .5f))
	{};

	//The colour of the letterbox, should have a high contrast compared to the text colours.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Colours")
		FLinearColor LetterboxColour;

	//The colour of the background behind the individual lines, should have a high contrast compared to the text colours.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Colours")
		FLinearColor LineBackColour;

	//The colour of the background behind the captions, should have a high contrast compared to the text colours.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Colours")
		FLinearColor CaptionBackColour;

	//Returns the subtitle text colour for the given speaker.
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|ColourProfile")
		FORCEINLINE FLinearColor const& GetSubtitleColour(FName Speaker) const
			{ return oGetSubtitleColour(Speaker); };

	//Returns the subtitle text colour for the given speaker.
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|ColourProfile")
		FORCEINLINE FLinearColor const& GetCaptionColour(FName Source) const
			{ return oGetCaptionColour(Source); };

protected:
	inline virtual FLinearColor const& oGetSubtitleColour(FName speaker) const
		{ return FLinearColor::White; };

	inline virtual FLinearColor const& oGetCaptionColour(FName source) const
		{ return FLinearColor::White; };
};

/**
 * 
 */
UCLASS()
class CRISPSUBTITLEFRAMEWORK_API UCSCPSimple : public UCSColourProfile
{
	GENERATED_BODY()

public:
	//The default text colour.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Colours")
		FLinearColor DefaultSubtitleTextColour = FLinearColor::White;

	//The default text colour.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Colours")
		FLinearColor DefaultCaptionTextColour = FLinearColor::White;

protected:
	virtual FLinearColor const& oGetSubtitleColour(FName speaker) const override;
	virtual FLinearColor const& oGetCaptionColour(FName source) const override;
};

/**
 * 
 */
UCLASS(Abstract)
class CRISPSUBTITLEFRAMEWORK_API UCSCPMatched : public UCSCPSimple
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|ColourProfile")
		FORCEINLINE bool HasColour(FName Speaker) const
			{ return oHasColour(Speaker); };

	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|ColourProfile")
		FORCEINLINE bool ColourWasMatched(FName Speaker) const
			{ return oColourWasMatched(Speaker); };

	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|ColourProfile")
		FORCEINLINE void LogMatch(FName Speaker)
			{ oLogMatch(Speaker); };

	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|ColourProfile")
		FORCEINLINE void ForgetMatch(FName Speaker)
			{ oForgetMatch(Speaker); };

protected:
	inline virtual bool oHasColour(FName speaker) const
		{ return false; };

	virtual bool oColourWasMatched(FName speaker) const;
	virtual void oLogMatch(FName speaker);
	virtual void oForgetMatch(FName speaker);

	TSet<FName> oMatchedSpeakers = TSet<FName>();
};

/**
 * 
 */
UCLASS()
class CRISPSUBTITLEFRAMEWORK_API UCSCPAssigned : public UCSCPMatched
{
	GENERATED_BODY()
		
public:
	//This Map matches speakers to their text colours. If the speaker has no assigned colour DefaultColour will be used.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ForceInlineRow), Category = "Colours")
		TMap<FName, FLinearColor> AssignedColours
		= {
			  { "Speaker1", FLinearColor::Yellow }
			, { "Speaker2", FLinearColor(0, 1, 1) }
			, { "Speaker3", FLinearColor::Green }
		};

protected:
	virtual bool oHasColour(FName speaker) const override;
	virtual FLinearColor const& oGetSubtitleColour(FName speaker) const override;
	virtual void oLogMatch(FName speaker) override;
};

/**
 * 
 */
UCLASS()
class CRISPSUBTITLEFRAMEWORK_API UCSCPCustom : public UCSCPAssigned
{
	GENERATED_BODY()
	
public:
	//
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|ColourProfile")
		void Empty();

	//
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|ColourProfile")
		void Forget(FName Speaker);

	//Returns the subtitle text colour for the given speaker.
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|ColourProfile")
		FORCEINLINE void AssignColour(FName Speaker, FLinearColor Colour)
			{ oAssignColour(Speaker, Colour); };

protected:
	void oAssignColour(FName speaker, FLinearColor colour);
};
