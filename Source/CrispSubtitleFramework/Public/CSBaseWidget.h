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

protected:
	virtual void eConstructExample(FVector2D const& size)
	{};
#endif
};
