// Copyright Crisp Clover.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CSBaseWidget.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class CRISPSUBTITLEFRAMEWORK_API UCSBaseWidget : public UUserWidget
{
	GENERATED_BODY()
	
#if WITH_EDITOR
public:
	virtual void SynchronizeProperties() override;
	virtual void SetDesignerFlags(EWidgetDesignFlags newFlags) override;
	virtual void OnDesignerChanged(const FDesignerChangedEventArgs& eventArgs) override;

	virtual inline const FText GetPaletteCategory() override
		{ return NSLOCTEXT("CrispSubtitles", "Subtitles", "Subtitles"); };

protected:
	virtual void eConstructExample(FVector2D const& size)
	{};
#endif
};
