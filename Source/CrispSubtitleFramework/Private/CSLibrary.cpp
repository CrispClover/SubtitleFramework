// Copyright Crisp Clover.

#include "CSLibrary.h"
#include "CSUserSettings.h"
#include "Engine/UserInterfaceSettings.h"

double UCSLibrary::AngleConversion(const double angle, const int32 segments, const EAngleUnit unit)
{
	const float twoPi = 6.2831855f;

	if (segments > 0)
	{
		const double approx = FMath::Floor((angle / twoPi) * segments + 0.5) / segments;
		switch (unit)
		{
		case EAngleUnit::degrees:
			return approx * 360;

		case EAngleUnit::turns:
			return approx;

		default:
			return approx * twoPi;
		}
	}
	else
	{
		switch (unit)
		{
		case EAngleUnit::degrees:
			return angle / twoPi * 360;

		case EAngleUnit::turns:
			return angle / twoPi;

		default:
			return angle;
		}
	}
}

FCrispSubtitle UCSLibrary::FrySubtitle(FFullSubtitle const& sub, const int32 id, UCSUserSettings const* settings)
{
	bool excludeSpeaker = settings->GetShowSpeaker(sub.SpeakerID) || sub.Speaker.IsEmpty();
	bool excludeDescription = !settings->bShowSubtitleDescriptions || sub.Description.IsEmpty();

	if (excludeSpeaker && excludeDescription)
		return FCrispSubtitle(FText(), sub.Lines, sub.SpeakerID, sub.Source, id);

	FText labelFormat;
	if (excludeDescription)
		labelFormat = settings->SpeakerOnlyLabelFormat;
	else if (excludeSpeaker)
		labelFormat = settings->DescriptionOnlyLabelFormat;
	else
		labelFormat = settings->FullLabelFormat;

	FText speakerM;
	if (settings->bSpeakersAreUpperCase)
		speakerM = sub.Speaker.ToUpper();
	else
		speakerM = sub.Speaker;

	FFormatNamedArguments args;
	args.Add("speaker", speakerM);
	args.Add("description", sub.Description);

	FText label = FText::Format(labelFormat, args);

	return FCrispSubtitle(label, sub.Lines, sub.SpeakerID, sub.Source, id);
}

FVector2D UCSLibrary::LocalPositionToNDC(FVector2D const& localPos, FIntPoint const& viewportSize)
{
	static TFrameValue<float> scaleCache;
	if (!scaleCache.IsSet() || WITH_EDITOR)
		scaleCache = GetDefault<UUserInterfaceSettings>()->GetDPIScaleBasedOnSize(viewportSize);

	const float scale = scaleCache.GetValue();

	return 2 * scale * (localPos / viewportSize) - 1;
}
