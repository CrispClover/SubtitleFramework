// Copyright Crisp Clover.

#include "CSBaseWidget.h"

#if WITH_EDITOR
void UCSBaseWidget::SynchronizeProperties()
{
	Super::SynchronizeProperties();
	
	if (IsDesignTime())
		eConstructExample(FVector2D());
}

void UCSBaseWidget::SetDesignerFlags(EWidgetDesignFlags newFlags)
{
	Super::SetDesignerFlags(newFlags);

	if (IsDesignTime())
		eConstructExample(FVector2D());
}

void UCSBaseWidget::OnDesignerChanged(const FDesignerChangedEventArgs& args)
{
	eConstructExample(args.Size / args.DpiScale);
}

#endif