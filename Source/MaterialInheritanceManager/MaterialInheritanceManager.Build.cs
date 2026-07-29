// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MaterialInheritanceManager : ModuleRules
{
	public MaterialInheritanceManager(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"InputCore",
				"Slate",
				"SlateCore",
				"UnrealEd",
				"EditorStyle",
				"EditorSubsystem",
				"PropertyEditor",
				"ToolMenus",
				"AssetTools",
				"AssetRegistry",
				"ContentBrowser",
				"ContentBrowserData",
				"LevelEditor",
				"WorkspaceMenuStructure",
				"Projects",
				"MaterialEditor",
				"ApplicationCore",
				"GraphEditor",
			}
		);

		if (Target.bBuildEditor == false)
		{
			PrivateDependencyModuleNames.RemoveAll(Module => Module != "Core");
		}
	}
}
