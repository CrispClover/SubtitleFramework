// Copyright Epic Games, Inc. All Rights Reserved.

#include "CrispSubtitleFramework.h"
#include "CSProjectSettings.h"
#include "ISettingsModule.h"

#define LOCTEXT_NAMESPACE "FCrispSubtitleFrameworkModule"

void FCrispSubtitleFrameworkModule::StartupModule()
{
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->RegisterSettings("Project", "Plugins", "CrispSubtitles",
			LOCTEXT("RuntimeSettingsName", "Crisp Subtitles"),
			LOCTEXT("RuntimeSettingsDescription", "Configure the Crisp Subtitles plugin"),
			GetMutableDefault<UCSProjectSettings>()
		);
	}
}

void FCrispSubtitleFrameworkModule::ShutdownModule()
{
	if (UObjectInitialized())
		if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
			SettingsModule->UnregisterSettings("Project", "Plugins", "CrispSubtitles");
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FCrispSubtitleFrameworkModule, CrispSubtitleFramework)