// Copyright Crisp Clover.

#pragma once

#include "CoreMinimal.h"
#include "CSTrackingManager.h"//TODO: Move
#include "CSTrackingManagerTick.h"//TODO: Move

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

	inline bool IsRegistered(const FName source) const
	{ return iSources.Contains(source); };

	inline bool AddSource(const FName name)
	{
		bool wasInSet = false;
		iSources.Add(name, &wasInSet);
		return !wasInSet;
	};

	bool RemoveSource(const FName name);

	bool TrackSound(FCSSoundID const& soundID, FVector const& location, ULocalPlayer const* player);

	void StopTrackingSource(const FName name, ULocalPlayer const* player);

	bool IsSoundTracked(FCSSoundID const& soundID, ULocalPlayer const* player) const;

	bool GetSoundData(FCSSoundID const& soundID, FVector& location, ULocalPlayer const* player) const;

	//Returns a pointer to the data for the indicator. Will be null if the source isn't tracked.
	CSIndicatorDelegates* RegisterIndicator(FCSRegisterArgs args, ULocalPlayer const* player);

	void UnregisterIndicator(FCSSoundID const& soundID, ULocalPlayer const* player, UObject* widget);

	void AddPlayer(ULocalPlayer const* player);

	void RemovePlayer(ULocalPlayer const* player);

	void UnionisePlayerSources(ULocalPlayer const* receivingPlayer, ULocalPlayer const* copiedPlayer);

private:
	CSTrackingManager const* iGetManager(ULocalPlayer const* player = nullptr) const;
	CSTrackingManager* iAccessManager(ULocalPlayer const* player = nullptr);

	CSTrackingManager* uTrackingManager = nullptr;
	TArray<CSTrackingManager*> iSplitscreenTrackingManagers = TArray<CSTrackingManager*>();

	TSet<FName> iSources = TSet<FName>();
	TSet<FName> iSourcesOverride = TSet<FName>();
};
