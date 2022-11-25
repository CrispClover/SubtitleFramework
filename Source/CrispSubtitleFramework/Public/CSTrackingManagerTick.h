// Copyright Crisp Clover.

#pragma once

#include "CSTrackingManager.h"

/**
 * 
 */
class CRISPSUBTITLEFRAMEWORK_API CSTrackingManagerTick : public CSTrackingManager, public FTickableGameObject
{
public:
	CSTrackingManagerTick() = delete;

	CSTrackingManagerTick(ULocalPlayer const* player)
		: CSTrackingManager(player)
	{};

	virtual void Tick(float dt) override
		{ Calculate(); };

	virtual ETickableTickType GetTickableTickType() const override
		{ return ETickableTickType::Always; };

	virtual TStatId GetStatId() const override
		{ RETURN_QUICK_DECLARE_CYCLE_STAT(CSTrackingManagerTick, STATGROUP_Tickables); };

	virtual bool IsTickableWhenPaused() const
		{ return true; };

	virtual bool IsTickableInEditor() const
		{ return false; };
};
