// Copyright Crisp Clover.

#include "CSProjectSettingFunctions.h"
#include "CSUserSettings.h"

#if WITH_EDITOR
#include "Engine/UserInterfaceSettings.h"

UCSUserSettings* UCSProjectSettingFunctions::GetDesignSettings(FVector2D const& screenSize)//TODO: move to editor tools once they exist.
{
	UCSUserSettings* settings = GetDefaultSettings();
	
	if (screenSize == FVector2D())
		settings->RecalculateDesignLayout(GetDefault<UUserInterfaceSettings>()->DesignScreenSize);
	else
		settings->RecalculateDesignLayout(screenSize);

	return settings;
};
#endif

TArray<FString> UCSProjectSettingFunctions::GetSettingsDirectories()
{
	TArray<FString> paths;

	for (FDirectoryPath const& path : GetDefault<UCSProjectSettings>()->SettingsDirectories)
		paths.Add(path.Path);

	return paths;
}

FCrispSubtitle UCSProjectSettingFunctions::GetExampleSubtitle(UCSUserSettings const* settings)
{
	return UCSCoreLibrary::FrySubtitle(GetDefault<UCSProjectSettings>()->ExampleSubtitle, INDEX_NONE, settings);
}

FCrispCaption UCSProjectSettingFunctions::GetExampleCaption()
{
	return FCrispCaption(GetDefault<UCSProjectSettings>()->ExampleCaption, INDEX_NONE);
}