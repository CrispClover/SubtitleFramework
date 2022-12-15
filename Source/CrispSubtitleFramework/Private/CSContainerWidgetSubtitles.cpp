// Copyright Crisp Clover.

#include "CSContainerWidgetSubtitles.h"
#include "CSS_SubtitleGISS.h"
#include "Kismet/GameplayStatics.h"
#include "CSUserSettings.h"
#include "CSUILibrary.h"
#include "CSLetterboxWidget.h"
#include "CSVerticalBox.h"
#include "CSVerticalBoxSlot.h"

#if WITH_EDITOR
void UCSContainerWidgetSubtitles::eConstructExample(FVector2D const& size)
{
	Super::eConstructExample(size);

	if (!Container)
		return;

	UCSUserSettings* settings = UCSProjectSettingFunctions::GetDesignSettings(FVector2D());
	TSubclassOf<UCSLetterboxWidget> letterboxClass = settings->LetterboxClass.LoadSynchronous();
	TArray<FCrispSubtitle> const& subtitles = UCSProjectSettingFunctions::GetExampleSubtitles(settings);
	
	for (UCSLetterboxWidget* example : eExamples)
		example->RemoveFromParent();

	eExamples.Empty();

	for (int32 x = 0; x < subtitles.Num(); x++)
	{
		UCSLetterboxWidget* example = CreateWidget<UCSLetterboxWidget>(this, letterboxClass);
		eExamples.Add(example);
		
		UCSVerticalBoxSlot* slot = Cast<UCSVerticalBoxSlot>(Container->AddChild(example));
		slot->SetPadding(settings->GetLayout().SubtitlePadding);

		FCSLetterboxStyle const& style = UCSUILibrary::GetDesignLetterboxStyle(subtitles[x].Speaker, subtitles[x].IsIndirectSpeech(), size);
		example->ConstructFromSubtitle(subtitles[x], style);
	}
}
#endif

void UCSContainerWidgetSubtitles::NativeConstruct()
{
	uCSS = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UCSS_SubtitleGISS>();

	uCSS->ConstructSubtitleEvent.AddDynamic(this, &UCSContainerWidgetSubtitles::OnSubtitleReceived);
	uCSS->DestroySubtitleEvent.AddDynamic(this, &UCSContainerWidgetSubtitles::OnDestroy);
	uCSS->ReconstructSubtitlesEvent.AddDynamic(this, &UCSContainerWidgetSubtitles::OnReconstruct);

	uCSS->RecalculateLayout();

	Super::NativeConstruct();
}

void UCSContainerWidgetSubtitles::NativeDestruct()
{
	uCSS->ConstructSubtitleEvent.RemoveAll(this);
	uCSS->DestroySubtitleEvent.RemoveAll(this);
	uCSS->ReconstructSubtitlesEvent.RemoveAll(this);

	Super::NativeDestruct();
}

UCSVerticalBoxSlot* UCSContainerWidgetSubtitles::GetSlot(const int32 id)
{
	if (!Container)
		return nullptr;
	else
		return Container->rFindSlot(id);
}

UCSLetterboxWidget* UCSContainerWidgetSubtitles::GetLetterbox(const int32 id)
{
	if (!Container)
		return nullptr;
	else
		return Cast<UCSLetterboxWidget>(Container->rFindChild(id));
}

void UCSContainerWidgetSubtitles::OnSubtitleReceived_Implementation(FCrispSubtitle const& subtitle)
{
	if (!uSettings && !uCSS)
		return;

	if(!uSettings)
		uSettings = uCSS->GetCurrentSettings();

	UCSLetterboxWidget* letterbox = CreateWidget<UCSLetterboxWidget>(this, uSettings->LetterboxClass.LoadSynchronous());
	UCSVerticalBoxSlot* slot = Container->FindOrAddSlot(letterbox, otNow(), uSettings->TimeGap, subtitle.ID);

	slot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);
	slot->SetPadding(uSettings->GetLayout().SubtitlePadding);
	letterbox->ConstructFromSubtitle(subtitle, UCSUILibrary::GetLetterboxStyle(uSettings, subtitle.Speaker, subtitle.IsIndirectSpeech()));
}

void UCSContainerWidgetSubtitles::OnDestroy_Implementation(const int32 id)
{
	if (!uSettings && !uCSS)
		return;

	if(!uSettings)
		uSettings = uCSS->GetCurrentSettings();
	
	FCSSpacerInfo spacerInfo = FCSSpacerInfo();//TODO
	spacerInfo.SpacerClass = uSettings->SubtitleSpacer.LoadSynchronous();
	
	const float dtMissing = Container->dtTryVacate(id, spacerInfo, otNow(), uSettings->TimeGap);

	if (dtMissing > 0.f)
	{
		if (UWorld* world = GetWorld())
		{
			FTimerHandle dropped;
			FTimerDelegate del = FTimerDelegate::CreateUObject(this, &UCSContainerWidgetSubtitles::OnDestroy, id);
			world->GetTimerManager().SetTimer(dropped, del, dtMissing, false);
		}
	}
}

void UCSContainerWidgetSubtitles::OnReconstruct_Implementation(TArray<FCrispSubtitle> const& subtitles, UCSUserSettings* settings)
{
	if (!settings || !Container)
		return;

	uSettings = settings;

	for(FCrispSubtitle const& subtitle : subtitles)
	{
		UCSLetterboxWidget* letterbox = CreateWidget<UCSLetterboxWidget>(this, uSettings->LetterboxClass.LoadSynchronous());
		
		if (UCSVerticalBoxSlot* slot = Container->rFindSlot(subtitle.ID))
		{
			slot->SetPadding(uSettings->GetLayout().CaptionPadding);

			if (UCSLetterboxWidget* widget = Cast<UCSLetterboxWidget>(slot->Content))
				widget->ConstructFromSubtitle(subtitle, UCSUILibrary::GetLetterboxStyle(uSettings, subtitle.Speaker, subtitle.IsIndirectSpeech()));
		}
	}
}