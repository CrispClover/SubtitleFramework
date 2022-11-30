// Copyright Crisp Clover.

#include "CSUserSettings.h"
#include "Engine/UserInterfaceSettings.h"

UCSUserSettings* UCSProjectSettingFunctions::GetDesignSettings()//TODO: move to editor tools once they exist.
{
	UCSUserSettings* settings = GetDefaultSettings();
	settings->RecalculateDesignLayout(GetDefault<UUserInterfaceSettings>()->DesignScreenSize);
	return settings;
};

FCrispSubtitle UCSProjectSettingFunctions::GetExampleSubtitle(UCSUserSettings const* settings)
{
	return UCSCoreLibrary::FrySubtitle(GetDefault<UCSProjectSettings>()->ExampleSubtitle, INDEX_NONE, settings);
}

FCrispCaption UCSProjectSettingFunctions::GetExampleCaption()
{
	return FCrispCaption(GetDefault<UCSProjectSettings>()->ExampleCaption, INDEX_NONE);
}