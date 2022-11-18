// Copyright Crisp Clover.

#include "CSProjectSettingFunctions.h"
#include "Engine/UserInterfaceSettings.h"

FCrispSubtitle UCSProjectSettingFunctions::GetExampleSubtitle(UCSUserSettings const* settings)
{
	return UCSLibrary::FrySubtitle(GetDefault<UCSProjectSettings>()->ExampleSubtitle, INDEX_NONE, settings);
}

UCSUserSettings* UCSProjectSettingFunctions::GetDesignSettings()
{
	UCSUserSettings* settings = GetDefault<UCSProjectSettings>()->DefaultSettings.LoadSynchronous();
	settings->RecalculateLayout(GetDefault<UUserInterfaceSettings>()->DesignScreenSize);
	return settings;
};