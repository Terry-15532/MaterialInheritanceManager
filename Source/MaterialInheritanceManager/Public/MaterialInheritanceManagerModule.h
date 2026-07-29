// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FMaterialInheritanceManagerModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	void OpenMainPanel();

private:
	void RegisterMenus();
	void RegisterMenus_Internal();
	void RegisterTabSpawner();
	void UnregisterTabSpawner();
	TSharedRef<class SDockTab> SpawnMainPanel(const class FSpawnTabArgs& Args);

	bool bCommandsRegistered = false;
};
