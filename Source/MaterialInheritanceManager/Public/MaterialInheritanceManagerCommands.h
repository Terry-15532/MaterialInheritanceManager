// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "MaterialInheritanceManagerStyle.h"

class FMaterialInheritanceManagerCommands : public TCommands<FMaterialInheritanceManagerCommands>
{
public:
	FMaterialInheritanceManagerCommands()
		: TCommands<FMaterialInheritanceManagerCommands>(
			TEXT("MaterialInheritanceManager"),
			NSLOCTEXT("Contexts", "MaterialInheritanceManager", "Material Inheritance Manager"),
			NAME_None,
			FMaterialInheritanceManagerStyle::GetStyleSetName())
	{
	}

	virtual void RegisterCommands() override;

	TSharedPtr<FUICommandInfo> OpenMainPanel;
};
