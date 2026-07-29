// Copyright Epic Games, Inc. All Rights Reserved.

#include "MaterialInheritanceManagerCommands.h"

#define LOCTEXT_NAMESPACE "MaterialInheritanceManagerCommands"

void FMaterialInheritanceManagerCommands::RegisterCommands()
{
	UI_COMMAND(OpenMainPanel, "Material Inheritance Manager", "Open the Material Inheritance Manager dockable panel.", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
