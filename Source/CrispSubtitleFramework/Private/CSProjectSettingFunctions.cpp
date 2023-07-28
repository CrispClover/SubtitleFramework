// Copyright Crisp Clover.

#include "CSProjectSettingFunctions.h"
#include "CSUserSettings.h"

#if WITH_EDITOR
#include "Engine/UserInterfaceSettings.h"

UCSUserSettings* UCSProjectSettingFunctions::GetDesignSettings(FVector2D const& screenSize)//TODO: move to editor tools once they exist.
{
	UCSUserSettings* settings = GetDefaultSettings();
	
	if (screenSize == FVector2D())
	{
		settings->RecalculateDesignLayout(GetDefault<UUserInterfaceSettings>()->DesignScreenSize);
	}
	else
	{
		settings->RecalculateDesignLayout(screenSize);
	}

	return settings;
};
#endif

TArray<FString> UCSProjectSettingFunctions::GetSettingsDirectories()
{
	TArray<FString> paths;

	for (FDirectoryPath const& path : GetDefault<UCSProjectSettings>()->SettingsDirectories)
	{
		paths.Add(path.Path);
	}

	return paths;
}

TArray<FCrispSubtitle> UCSProjectSettingFunctions::GetExampleSubtitles(UCSUserSettings const* settings)
{
	TArray<FCrispSubtitle> subtitles;

	for (FFullSubtitle const& subtitle : GetDefault<UCSProjectSettings>()->ExampleSubtitles)
	{
		subtitles.Add(UCSCoreLibrary::FrySubtitle(subtitle, INDEX_NONE, settings));
	}

	return subtitles;
}

TArray<FCrispCaption> UCSProjectSettingFunctions::GetExampleCaptions()
{
	TArray<FCrispCaption> captions;

	for (FFullCaption const& caption : GetDefault<UCSProjectSettings>()->ExampleCaptions)
	{
		captions.Add(FCrispCaption(caption, INDEX_NONE));
	}

	return captions;
}