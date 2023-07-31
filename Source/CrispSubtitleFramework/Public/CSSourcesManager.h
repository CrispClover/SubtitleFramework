// Copyright Crisp Clover.

#pragma once

#include "CoreMinimal.h"
#include "CSTrackingManager.h"

/**
 * 
 */
class CRISPSUBTITLEFRAMEWORK_API FCSSourcesManager
{

public:
	FCSSourcesManager()
	{};

	~FCSSourcesManager();
	
	void CallCalculate(ULocalPlayer const* player);

	void Clear();
	void EmptySources();
	void RebuildPlayers(TArray<ULocalPlayer*> const& players);

	//We can expect memory usage to plateau, so we mostly shrink manually. (splitscreenTrackingManagers shrinks itself)
	void Shrink();

	inline TSet<FName> GetSources() const
	{
		if (HasOverride())
			return iSourcesOverride;

		return iSources;
	};

	inline void SetSourcesOverride(TSet<FName> const& Override)
		{ iSourcesOverride = Override.Intersect(iSources); };

	inline void ClearSourcesOverride()
		{ iSourcesOverride.Empty(); };

	inline bool HasOverride() const
		{ return !iSourcesOverride.IsEmpty(); };

	inline TSet<FName> const& GetOverride() const
		{ return iSourcesOverride; };

	inline bool IsRegistered(const FName source) const
		{ return iSources.Contains(source); };

	inline bool AddSource(const FName name)
	{
		bool wasInSet = false;
		iSources.Add(name, &wasInSet);
		return !wasInSet;
	};

	bool RemoveSource(const FName name);

	bool TrackSound(FCSSoundID const& soundID, FVector const& pSound, ULocalPlayer const* player);

	void StopTrackingSound(FCSSoundID const& soundID, ULocalPlayer const* player);

	void StopTrackingSource(const FName name, ULocalPlayer const* player);

	bool IsSoundTracked(FCSSoundID const& soundID, ULocalPlayer const* player) const;

	bool GetSoundData(FCSSoundID const& soundID, FVector& pSound, ULocalPlayer const* player) const;

	TArray<FVector> GetSoundDataDump(TArray<FCSSoundID>& soundIDs, ULocalPlayer const* player) const;

	template<typename UserClass>
	void RegisterIndicator(CSIndicatorRegistrationData<UserClass> args, ULocalPlayer const* player)
	{
		if (CSTrackingManager* manager = uAccessManager(player))
			manager->RegisterIndicator(args);
	}

	void UnregisterIndicator(FCSSoundID const& soundID, ULocalPlayer const* player, UObject* widget);

	void AddPlayer(ULocalPlayer const* player);

	void RemovePlayer(ULocalPlayer const* player);

	void UnionisePlayerSources(ULocalPlayer const* receivingPlayer, ULocalPlayer const* copiedPlayer);

private:
	CSTrackingManager const* uGetManager(ULocalPlayer const* player = nullptr) const;
	CSTrackingManager* uAccessManager(ULocalPlayer const* player = nullptr);

	CSTrackingManager* uTrackingManager = nullptr;
	TArray<CSTrackingManager*> iSplitscreenTrackingManagers = TArray<CSTrackingManager*>();

	TSet<FName> iSources = TSet<FName>();
	TSet<FName> iSourcesOverride = TSet<FName>();
};
