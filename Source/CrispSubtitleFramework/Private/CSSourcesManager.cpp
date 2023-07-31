// Copyright Crisp Clover.

#include "CSSourcesManager.h"
#include "CSProjectSettingFunctions.h"
#include "CSTrackingManagerTick.h"

FCSSourcesManager::~FCSSourcesManager()
{
	Clear();
}

void FCSSourcesManager::CallCalculate(ULocalPlayer const* player)
{
	if (UCSProjectSettingFunctions::SupportSplitscreen() && !player)
	{
		for (CSTrackingManager* manager : iSplitscreenTrackingManagers)
			manager->Calculate();
	}
	else
	{
		if (CSTrackingManager* manager = uAccessManager(player))
		{
			manager->Calculate();
		}
	}
}

void FCSSourcesManager::Clear()
{
	iSources.Empty();
	iSourcesOverride.Empty();

	if (UCSProjectSettingFunctions::SupportSplitscreen())
	{
		for (CSTrackingManager* manager : iSplitscreenTrackingManagers)
			delete manager;

		iSplitscreenTrackingManagers.Empty();
	}
	else
	{
		if (uTrackingManager)
		{
			delete uTrackingManager;
			uTrackingManager = nullptr;
		}
	}
}

void FCSSourcesManager::EmptySources()
{
	iSources.Empty();
	iSourcesOverride.Empty();

	if (UCSProjectSettingFunctions::SupportSplitscreen())
	{
		for (int32 i = 1; i < iSplitscreenTrackingManagers.Num(); i++)
			iSplitscreenTrackingManagers[i]->Empty();
	}
	else
	{
		if (uTrackingManager)
		{
			uTrackingManager->Empty();
		}
	}
}

void FCSSourcesManager::RebuildPlayers(TArray<ULocalPlayer*> const& players)
{
	if (players.IsEmpty())
		return;
	
	if (UCSProjectSettingFunctions::SupportSplitscreen())
	{
		for (CSTrackingManager* manager : iSplitscreenTrackingManagers)
			delete manager;

		iSplitscreenTrackingManagers.Empty();

		for (ULocalPlayer const* player : players)
			AddPlayer(player);
	}
	else
	{
		AddPlayer(players[0]);
	}
}

void FCSSourcesManager::Shrink()
{
	if (UCSProjectSettingFunctions::SupportSplitscreen())
	{
		for (CSTrackingManager* manager : iSplitscreenTrackingManagers)
			manager->Shrink();
	}
	else
	{
		if (uTrackingManager)
		{
			uTrackingManager->Shrink();
		}
	}

	iSources.Compact();
	iSourcesOverride.Compact();
}

bool FCSSourcesManager::RemoveSource(FName name)
{
	if (!iSources.Remove(name))
		return false;

	if (UCSProjectSettingFunctions::SupportSplitscreen())
	{
		for (CSTrackingManager* manager : iSplitscreenTrackingManagers)
			manager->RemoveSource(name);
	}
	else
	{
		if (CSTrackingManager* manager = uAccessManager())
		{
			manager->RemoveSource(name);
		}
	}

	return true;
}

bool FCSSourcesManager::TrackSound(FCSSoundID const& soundID, FVector const& pSound, ULocalPlayer const* player)
{
	if (!IsRegistered(soundID.Source))
		return false;

	if (UCSProjectSettingFunctions::SupportSplitscreen() && !player)
	{
		for (CSTrackingManager* manager : iSplitscreenTrackingManagers)
			manager->TrackSound(soundID, pSound);
	}
	else
	{
		if (CSTrackingManager* manager = uAccessManager(player))
		{
			manager->TrackSound(soundID, pSound);
		}
	}

	return true;
}

void FCSSourcesManager::StopTrackingSound(FCSSoundID const& soundID, ULocalPlayer const* player)
{
	if (UCSProjectSettingFunctions::SupportSplitscreen() && !player)
	{
		for (CSTrackingManager* manager : iSplitscreenTrackingManagers)
			manager->RemoveSound(soundID);
	}
	else
	{
		if (CSTrackingManager* manager = uAccessManager(player))
		{
			manager->RemoveSound(soundID);
		}
	}
}

void FCSSourcesManager::StopTrackingSource(const FName name, ULocalPlayer const* player)
{
	if (UCSProjectSettingFunctions::SupportSplitscreen() && !player)
	{
		for (CSTrackingManager* manager : iSplitscreenTrackingManagers)
			manager->RemoveSource(name);
	}
	else
	{
		if (CSTrackingManager* manager = uAccessManager(player))
		{
			manager->RemoveSource(name);
		}
	}
}

bool FCSSourcesManager::IsSoundTracked(FCSSoundID const& soundID, ULocalPlayer const* player) const
{
	if (UCSProjectSettingFunctions::SupportSplitscreen() && !player)
	{
		for (CSTrackingManager const* manager : iSplitscreenTrackingManagers)
			if (manager->Contains(soundID))
				return true;

		return false;
	}
	else
	{
		if (CSTrackingManager const* manager = uGetManager(player))
			return manager->Contains(soundID);
		else
			return false;
	}
}

bool FCSSourcesManager::GetSoundData(FCSSoundID const& soundID, FVector& pSound, ULocalPlayer const* player) const
{
	if (CSTrackingManager const* manager = uGetManager(player))
		return manager->GetSoundData(soundID, pSound);
	else
		return false;
}

TArray<FVector> FCSSourcesManager::GetSoundDataDump(TArray<FCSSoundID>& soundIDs, ULocalPlayer const* player) const
{
	TArray<FVector> result = TArray<FVector>();
	
	if (UCSProjectSettingFunctions::SupportSplitscreen() && !player)
	{
		for (CSTrackingManager const* manager : iSplitscreenTrackingManagers)
			manager->GetSoundDataDump(soundIDs, result);
	}
	else
	{
		if (CSTrackingManager const* manager = uGetManager(player))
		{
			manager->GetSoundDataDump(soundIDs, result);
		}
	}
	
	return result;
}

void FCSSourcesManager::UnregisterIndicator(FCSSoundID const& soundID, ULocalPlayer const* player, UObject* widget)
{
	if (CSTrackingManager* manager = uAccessManager(player))
	{
		manager->UnregisterIndicator(soundID, widget);
	}
}

void FCSSourcesManager::AddPlayer(ULocalPlayer const* player)
{
	if (UCSProjectSettingFunctions::SupportSplitscreen())
	{
		if (UCSProjectSettingFunctions::CalculateIndicatorsOnTick())
		{
			iSplitscreenTrackingManagers.Add(new CSTrackingManagerTick(player));
		}
		else
		{
			iSplitscreenTrackingManagers.Add(new CSTrackingManager(player));
		}
	}
	else if (!uTrackingManager)
	{
		if (UCSProjectSettingFunctions::CalculateIndicatorsOnTick())
		{
			uTrackingManager = new CSTrackingManagerTick(player);
		}
		else
		{
			uTrackingManager = new CSTrackingManager(player);
		}
	}
}

void FCSSourcesManager::RemovePlayer(ULocalPlayer const* player)
{
	if (UCSProjectSettingFunctions::SupportSplitscreen())
	{
		for (int32 i = iSplitscreenTrackingManagers.Num() - 1; i >= 0; i--)
		{
			if (iSplitscreenTrackingManagers[i]->Player == player)
			{
				delete iSplitscreenTrackingManagers[i];
				iSplitscreenTrackingManagers.RemoveAtSwap(i);
				return;
			}
		}
	}
	else
	{
		if (uTrackingManager)
		{
			delete uTrackingManager;
			uTrackingManager = nullptr;
		}
	}
}

void FCSSourcesManager::UnionisePlayerSources(ULocalPlayer const* receivingPlayer, ULocalPlayer const* copiedPlayer)
{
	if (!UCSProjectSettingFunctions::SupportSplitscreen() || iSplitscreenTrackingManagers.Num() < 2 || !receivingPlayer || !copiedPlayer)
		return;

	CSTrackingManager const* copying = uGetManager(copiedPlayer);
	CSTrackingManager* receiving = uAccessManager(receivingPlayer);

	if (copying && receiving)
	{
		receiving->vCopy(copying);
	}
}

CSTrackingManager const* FCSSourcesManager::uGetManager(ULocalPlayer const* player) const
{
	if (UCSProjectSettingFunctions::SupportSplitscreen() && player)
	{
		for (CSTrackingManager const* manager : iSplitscreenTrackingManagers)
			if (manager->Player == player)
				return manager;
	}

	return uTrackingManager;
}

CSTrackingManager* FCSSourcesManager::uAccessManager(ULocalPlayer const* player)
{
	if (UCSProjectSettingFunctions::SupportSplitscreen() && player)
	{
		for (CSTrackingManager* manager : iSplitscreenTrackingManagers)
			if (manager->Player == player)
				return manager;
	}

	return uTrackingManager;
}