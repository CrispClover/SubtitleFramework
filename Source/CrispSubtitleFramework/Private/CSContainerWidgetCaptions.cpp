// Copyright Crisp Clover.

#include "CSContainerWidgetCaptions.h"
#include "CSS_SubtitleGISS.h"
#include "Kismet/GameplayStatics.h"
#include "CSUserSettings.h"
#include "CSCaptionWidget.h"
#include "CSUILibrary.h"
#include "CSVerticalBox.h"
#include "CSVerticalBoxSlot.h"
#include "CSBaseSpacer.h"

#if WITH_EDITOR
void UCSContainerWidgetCaptions::eConstructExample(FVector2D const& size)
{
	Super::eConstructExample(size);

	if (!Container)
		return;
	
	UCSUserSettings* settings = UCSProjectSettingFunctions::GetDesignSettings(FVector2D());
	TSubclassOf<UCSCaptionWidget> captionClass = settings->CaptionClass.LoadSynchronous();
	TArray<FCrispCaption> const& captions = UCSProjectSettingFunctions::GetExampleCaptions();
		
	for (UCSCaptionWidget* example : eExamples)
	{
		if (Container->HasChild(example))
		{
			example->RemoveFromParent();
		}
	}

	eExamples.Empty();

	for (int32 x = 0; x < captions.Num(); x++)
	{
		UCSCaptionWidget* example = CreateWidget<UCSCaptionWidget>(this, captionClass);
		eExamples.Add(example);

		UCSVerticalBoxSlot* slot = Cast<UCSVerticalBoxSlot>(Container->AddDesignChild(example));
		slot->SetPadding(settings->GetLayout().CaptionPadding);
		slot->SetHorizontalAlignment(settings->CaptionAlignment);

		FCSCaptionStyle const& style = UCSUILibrary::GetDesignCaptionStyle(captions[x].SoundID.Source, size);
		example->ConstructFromCaption(captions[x], UCSUILibrary::GetCaptionStyle(settings, captions[x].SoundID.Source));
	}
}
#endif

void UCSContainerWidgetCaptions::NativeConstruct()
{
	uCSS = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UCSS_SubtitleGISS>();

	uCSS->ConstructCaptionEvent.AddDynamic(this, &UCSContainerWidgetCaptions::OnCaptionReceived);
	uCSS->DestructCaptionEvent.AddDynamic(this, &UCSContainerWidgetCaptions::OnDestroy);
	uCSS->ReconstructCaptionsEvent.AddDynamic(this, &UCSContainerWidgetCaptions::OnReconstruct);

	uCSS->RecalculateLayout();

	Super::NativeConstruct();
}

void UCSContainerWidgetCaptions::NativeDestruct()
{
	uCSS->ConstructCaptionEvent.RemoveAll(this);
	uCSS->DestructCaptionEvent.RemoveAll(this);
	uCSS->ReconstructCaptionsEvent.RemoveAll(this);

	Super::NativeDestruct();
}

UCSVerticalBoxSlot* UCSContainerWidgetCaptions::GetSlot(const int32 id)
{
	if (!Container)
		return nullptr;
	else
		return Container->uFindSlot(id);
}

UCSCaptionWidget* UCSContainerWidgetCaptions::GetCaptionWidget(const int32 id)
{
	if (!Container)
		return nullptr;
	else
		return Cast<UCSCaptionWidget>(Container->uFindChild(id));
}

void UCSContainerWidgetCaptions::OnCaptionReceived_Implementation(FCrispCaption const& caption)
{
	if (!Container || !uSettings && !uCSS)
		return;

	if(!uSettings)
	{
		uSettings = uCSS->GetCurrentSettings();
	}

	const FCSCaptionStyle style = UCSUILibrary::GetCaptionStyle(uSettings, caption.SoundID.Source);

	UCSCaptionWidget* captionWidget = CreateWidget<UCSCaptionWidget>(this, uSettings->CaptionClass.LoadSynchronous());

	UCSVerticalBoxSlot* slot = Container->FindOrAddSlot(captionWidget, tNow(), uSettings->TimeGap, caption.ID);
	slot->SetPadding(uSettings->GetLayout().CaptionPadding);
	slot->SetHorizontalAlignment(uSettings->CaptionAlignment);

	captionWidget->ConstructFromCaption(caption, style);
}

void UCSContainerWidgetCaptions::OnDestroy_Implementation(const int32 id)
{
	if (!Container || !uSettings && !uCSS)
		return;

	if (!uSettings)
	{
		uSettings = uCSS->GetCurrentSettings();
	}

	const float dtMissing = Container->dtTryVacate(id, uSettings->CaptionSpacer.LoadSynchronous(), tNow(), uSettings->TimeGap);

	if (dtMissing > 0.f)
	{
		if (UWorld* world = GetWorld())
		{
			FTimerHandle dropped;
			FTimerDelegate del = FTimerDelegate::CreateUObject(this, &UCSContainerWidgetCaptions::OnDestroy, id);
			world->GetTimerManager().SetTimer(dropped, del, dtMissing, false);
		}
	}
}

void UCSContainerWidgetCaptions::OnReconstruct_Implementation(TArray<FCrispCaption> const& captions, UCSUserSettings* settings)
{
	if (!settings || !Container)
		return;

	uSettings = settings;

	for (FCrispCaption const& caption : captions)
	{
		if (UCSVerticalBoxSlot* slot = Container->uFindSlot(caption.ID))
		{
			slot->SetPadding(uSettings->GetLayout().CaptionPadding);
			slot->SetHorizontalAlignment(uSettings->CaptionAlignment);

			if (UCSCaptionWidget* widget = Cast<UCSCaptionWidget>(slot->Content))
			{
				widget->ConstructFromCaption(caption, UCSUILibrary::GetCaptionStyle(uSettings, caption.SoundID.Source));
			}
		}
	}
}