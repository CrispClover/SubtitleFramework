// Copyright Crisp Clover.

#include "CrispSubtitleFramework.h"
#include "CSProjectSettings.h"
#include "ISettingsModule.h"

#define LOCTEXT_NAMESPACE "FCrispSubtitleFrameworkModule"

void FCrispSubtitleFrameworkModule::StartupModule()
{
	if (ISettingsModule* settingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		settingsModule->RegisterSettings("Project", "Plugins", "CrispSubtitles",
			LOCTEXT("RuntimeSettingsName", "Crisp Subtitles"),
			LOCTEXT("RuntimeSettingsDescription", "Configure the Crisp Subtitles plugin"),
			GetMutableDefault<UCSProjectSettings>()
		);
	}
}

void FCrispSubtitleFrameworkModule::ShutdownModule()
{
	if (!UObjectInitialized())
		return;
	
	if (ISettingsModule* settingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		settingsModule->UnregisterSettings("Project", "Plugins", "CrispSubtitles");
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FCrispSubtitleFrameworkModule, CrispSubtitleFramework)