// Copyright Crisp Clover. Feel free to copy.

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
	//Returns the subtitle text colour for the given speaker.
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|ColourProfile")
		FORCEINLINE FLinearColor const& GetColour(FName Speaker) const
			{ return oGetColour(Speaker); };

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
	virtual FLinearColor const& oGetColour(FName speaker) const
		{ return FLinearColor::White; };

	inline virtual bool oColourWasMatched(FName speaker) const
		{ return false; };
	
	inline virtual void oLogMatch(FName speaker)
		{ return; };

	inline virtual void oForgetMatch(FName speaker)
		{ return; };
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
		FLinearColor DefaultColour = FLinearColor::White;

protected:
	virtual FLinearColor const& oGetColour(FName speaker) const override;
};

/**
 * 
 */
UCLASS()
class CRISPSUBTITLEFRAMEWORK_API UCSCPAssigned : public UCSCPSimple
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
	virtual FLinearColor const& oGetColour(FName speaker) const override;
	virtual bool oColourWasMatched(FName speaker) const override;
	virtual void oLogMatch(FName speaker) override;
	virtual void oForgetMatch(FName speaker) override;

	TSet<FName> oMatchedSpeakers = TSet<FName>();
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
