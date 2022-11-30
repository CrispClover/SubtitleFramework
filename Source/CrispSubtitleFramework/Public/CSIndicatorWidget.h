// Copyright Crisp Clover. 

#pragma once

#include "CoreMinimal.h"
#include "CSS_SubtitleGISS.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "CSIndicatorWidget.generated.h"

/*
 * 
 */
UCLASS(Abstract)
class CRISPSUBTITLEFRAMEWORK_API UCSIndicatorWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(FGeometry const& MyGeometry, float InDeltaTime) override;
	virtual void NativeDestruct() override;

public:
	//
	UPROPERTY(BlueprintReadOnly, Category = "CrispSubtitles|UI", meta = (BindWidget))
		UImage* Image = nullptr;

	//The number of segments used to limit indicator accuracy.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CrispSubtitles|Settings")
		int32 Segments = 0;

	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Data")
		bool GetIndicatorData(FCSIndicatorWidgetData& WidgetData) const;
	
	//If the object is behind the camera, this value will be negative. Magnitude is the screen-distance between widget and object.
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Data")
		FORCEINLINE float GetOpacityDriver() const
	{ return uWidgetData ? uWidgetData->OpacityDriver : 0; };
	
	//Gets the ID for the sound this indicator points toward. ID is set using "Register".
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Data")
		FORCEINLINE FCSSoundID GetSoundID()
			{ return iSoundID; };
	
	//TODO
	UFUNCTION(BlueprintCallable, Category = "CrispSubtitles|Data")
		void UpdateCenterPosition(FGeometry const& MyGeometry);

	//Called every time the TrackingManager has calculated new values for the indicators
	UFUNCTION(BlueprintNativeEvent, Category = "CrispSubtitles|Events")
		void OnUpdateIndicators();

	//
	UFUNCTION(BlueprintNativeEvent, Category = "CrispSubtitles|Events")
		void Register(FCSSoundID const& SoundID);

protected:
	virtual void OnUpdateIndicators_Implementation();
	void Register_Implementation(FCSSoundID const& id);
	UCSS_SubtitleGISS* oCSS = nullptr;

private:
	void iUpdateOffset() const;
	void iUpdateDataPtr(FCSSwapArgs const& a);
	
	FCSSoundID iSoundID;
	FVector2D iCenterPos;
	FCSIndicatorWidgetData* uWidgetData;
};
