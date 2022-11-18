// Copyright Crisp Clover.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CSLibrary.generated.h"

class UCSUserSettings;

UENUM(BlueprintType)
enum class EAngleUnit : uint8
{
	degrees,
	turns,
	radians
};

#pragma region STRUCTS

USTRUCT(BlueprintType)
struct FCSSoundID
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FName Source;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FName Sound;

	FCSSoundID()
		: Source()
		, Sound()
	{};

	FCSSoundID(const FName source, const FName sound)
		: Source(source)
		, Sound(sound)
	{};

    inline bool operator==(FCSSoundID const& other) const
    {
		return Equals(other);
    }

    inline bool operator!=(FCSSoundID const& other) const
    {
		return !Equals(other);
    }

    inline bool Equals(FCSSoundID const& other) const
    {
		return Source == other.Source && Sound == other.Sound;
    }
};

FORCEINLINE uint32 GetTypeHash(FCSSoundID const& id)//TODO
{
	uint32 srcHash = GetTypeHash(id.Source);//R
	uint32 sndHash = GetTypeHash(id.Sound);//N

	return (srcHash & uint32(2863311530)) ^ (sndHash & uint32(1431655765));//RNRN RNRN [...] for 32 bits
};

//The structure of a sound caption.
USTRUCT(BlueprintType)
struct FSoundCaption
{
	GENERATED_BODY()

public:
	//The content of the caption.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CrispSubtitles")
		FText Description;

	//The time in seconds after which the caption should begin to be displayed.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CrispSubtitles")
		float StartDelay;

	FSoundCaption()
		: Description(FText())
		, StartDelay(0)
	{};

	FSoundCaption(FText const& description)
		: Description(description)
		, StartDelay(0)
	{};

	FSoundCaption(FText const& description, const float startTime)
		: Description(description)
		, StartDelay(startTime)
	{};
};

//The full structure of a sound caption.
USTRUCT(BlueprintType)
struct FFullCaption : public FSoundCaption
{
	GENERATED_BODY()

public:
	//Contains the ID to identify the sound source.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CrispSubtitles")
		FCSSoundID SoundID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CrispSubtitles")
		float DisplayDuration;

	FFullCaption()
		: FSoundCaption()
		, SoundID()
		, DisplayDuration(1.5f)
	{};

	FFullCaption(FSoundCaption const& caption, const FName source, const FName sound)
		: FSoundCaption(caption)
		, SoundID(source, sound)
		, DisplayDuration(1.5f)
	{};

	FFullCaption(FSoundCaption const& caption, const FName source, const FName sound, const float duration)
		: FSoundCaption(caption)
		, SoundID(source, sound)
		, DisplayDuration(duration)
	{};
};

//The structure of a raw subtitle.
USTRUCT(BlueprintType)
struct FRawSubtitle : public FSoundCaption
{
	GENERATED_BODY()

public:
	/**
	 * The estimated time a user will read this subtitle. This plugin comes with functions to help calculate that time.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CrispSubtitles")
		float ReadDuration;

	/**
	 * The lines to be displayed as a subtitle. For better accessibility you should not display more than two lines in one subtitle, three are possible as an exception.
	 * Additional information can be stored in additional indeces using a prefix and should be consumed before displaying the subtitle.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CrispSubtitles")
		TArray<FText> Lines;

	FRawSubtitle()
		: FSoundCaption()
		, ReadDuration(1.5f)
		, Lines(TArray<FText>{FText::FromString("We would like to show you something,"), FText::FromString("but this subtitle is empty.")})
	{};

	FRawSubtitle(FText const& description, const float displayDuration)
		: FSoundCaption()
		, ReadDuration(displayDuration)
		, Lines(TArray<FText>{description})
	{};

	FRawSubtitle(FSoundCaption const& caption, TArray<FText> const& lines, const float readDuration)
		: FSoundCaption(caption)
		, ReadDuration(readDuration)
		, Lines(lines)
	{};
};

//The structure of a raw subtitle with multiple speakers. Contains the speaker information instead of constructing it.
USTRUCT(BlueprintType)
struct FGroupSubtitle : public FRawSubtitle
{
	GENERATED_BODY()

public:
	//Contains the information on who is speaking.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CrispSubtitles")
		FText Speaker;

	//Contains the ID to identify the speaker, which may differ from the source.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CrispSubtitles")
		FName SpeakerID;

	FGroupSubtitle()
		: FRawSubtitle()
		, Speaker(FText::FromString("error"))
		, SpeakerID(FName("error"))
	{};

	FGroupSubtitle(FText const& description, const float displayDuration)
		: FRawSubtitle(description, displayDuration)
		, Speaker()
		, SpeakerID()
	{};

	FGroupSubtitle(FRawSubtitle const& raw, FText const& speaker, FName speakerID)
		: FRawSubtitle(raw)
		, Speaker(speaker)
		, SpeakerID(speakerID)
	{};
};

//The structure of all the data required to construct a subtitle.
USTRUCT(BlueprintType)
struct FFullSubtitle : public FGroupSubtitle
{
	GENERATED_BODY()

public:
	//Contains the ID to identify the sound source.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CrispSubtitles")
		FName Source;

	FFullSubtitle()
		: FGroupSubtitle()
		, Source(FName("error"))
	{};

	FFullSubtitle(FRawSubtitle const& raw, FText speaker, FName speakerID,  const FName source)
		: FGroupSubtitle(raw, speaker, speakerID)
		, Source(source)
	{};

	FFullSubtitle(FGroupSubtitle const& group, FName source)
		: FGroupSubtitle(group)
		, Source(source)
	{};

	FFullSubtitle(FFullSubtitle const& subtitle, const float startTimeOverride)
		: FFullSubtitle(subtitle)
	{
		StartDelay = startTimeOverride;
	};

	FFullSubtitle(FFullCaption const& caption)
		: FGroupSubtitle(caption.Description, caption.DisplayDuration)
		, Source(FName(caption.SoundID.Source))
	{};
};

//The structure of a sound caption, ready to be displayed.
USTRUCT(BlueprintType)
struct FCrispCaption
{
	GENERATED_BODY()

public:
	//The content of the caption.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CrispSubtitles")
		FText Description;

	//An ID to indicate direction, etc.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CrispSubtitles")
		FCSSoundID SoundID;

	/**
	 * The ID assigned by the subtitle subsystem.
	 * The IDs of captions and subtitles do not collide and are reused when captions/subtitles are removed.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CrispSubtitles")
		int32 ID;
	
	//TODO: dur? max 2-2.5s dur (for calc)
	FCrispCaption()
		: Description()
		, SoundID()
		, ID()
	{};

	FCrispCaption(FFullCaption const& caption, const int32 id)
		: Description(caption.Description)
		, SoundID(caption.SoundID)
		, ID(id)
	{};
};

//The structure of a subtitle, ready to be displayed.
USTRUCT(BlueprintType)
struct FCrispSubtitle
{
	GENERATED_BODY()

public:
	//The label
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CrispSubtitles")
		FText Label;

	//The lines as they will be rendered.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CrispSubtitles")
		TArray<FText> Lines;

	//An ID for colour-coding the subtitle, indicating direction, etc.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CrispSubtitles")
		FName Speaker;

	//Another ID ... TODO
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CrispSubtitles")
		FName Source;

	/**
	 * The ID assigned by the subtitle subsystem.
	 * The IDs of captions and subtitles do not collide and are reused when captions/subtitles are removed.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CrispSubtitles")
		int32 ID;

	FCrispSubtitle()
		: Label()
		, Lines(TArray<FText>())
		, Speaker(FName())
		, Source(FName())
		, ID()
	{};

	FCrispSubtitle(FText const& label, TArray<FText> const& lines, const FName speakerID, const FName source, const int32 id)
		: Label(label)
		, Lines(lines)
		, Speaker(speakerID)
		, Source(source)
		, ID(id)
	{};
};

#pragma endregion

/**
 * 
 */
UCLASS()
class CRISPSUBTITLEFRAMEWORK_API UCSLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	/**
	 * Converts an angle for usage on indicators.
	 * @param Angle The angle to convert (in radians).
	 * @param Segments Divides the full circle rotations into steps of "360/Segments" degrees. (Only for numbers above 0.)
	 * @param Unit The angle unit you want returned.
	 */
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Data")
		static double AngleConversion(const double Angle, const int32 Segments = 0, const EAngleUnit Unit = EAngleUnit::degrees);

	/**
	 * Prepares a subtitle for being displayed in the UI.
	 * @param Subtitle The subtitle to prepare.
	 * @param ID The ID of the subtitle.
	 * @param Settings The settings to apply to the label.
	 * @return A crisp subtitle
	 */
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Preparation")
		static FCrispSubtitle FrySubtitle(FFullSubtitle const& Subtitle, const int32 ID, UCSUserSettings const* Settings);

	//TODO
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|TODO")
		static FVector2D LocalPositionToNDC(FVector2D const& LocalPosition, FIntPoint const& ViewportSize);
};
