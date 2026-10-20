// Copyright Epic Games, Inc. All Rights Reserved.

#include "MaterialInheritanceManagerModule.h"
#include "MaterialInheritanceManagerStyle.h"
#include "MaterialInheritanceManagerCommands.h"
#include "SMaterialInheritancePanel.h"

#include "Modules/ModuleManager.h"
#include "Framework/Application/SlateApplication.h"
#include "Widgets/Docking/SDockTab.h"
#include "Framework/Docking/TabManager.h"
#include "ToolMenus.h"
#include "ToolMenuSection.h"
#include "ToolMenuMisc.h"
#include "Widgets/Input/SButton.h"
#include "Interfaces/IPluginManager.h"
#include "HAL/PlatformProcess.h"
#include "Misc/Paths.h"

#define LOCTEXT_NAMESPACE "MaterialInheritanceManagerModule"

static const FName MaterialInheritancePanelTabId(TEXT("MaterialInheritanceManagerTab"));

void FMaterialInheritanceManagerModule::StartupModule()
{
	FMaterialInheritanceManagerStyle::Initialize();

	FMaterialInheritanceManagerCommands::Register();
	bCommandsRegistered = true;

	RegisterTabSpawner();
	RegisterMenus();
}

void FMaterialInheritanceManagerModule::ShutdownModule()
{
	if (UToolMenus::IsToolMenuUIEnabled())
	{
		UToolMenus::UnRegisterStartupCallback(this);
		UToolMenus::UnregisterOwner(this);
	}

	UnregisterTabSpawner();

	if (bCommandsRegistered)
	{
		FMaterialInheritanceManagerCommands::Unregister();
		bCommandsRegistered = false;
	}

	FMaterialInheritanceManagerStyle::Shutdown();
}

void FMaterialInheritanceManagerModule::RegisterTabSpawner()
{
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(
		MaterialInheritancePanelTabId,
		FOnSpawnTab::CreateRaw(this, &FMaterialInheritanceManagerModule::SpawnMainPanel))
		.SetDisplayName(LOCTEXT("TabTitle", "材质继承管理器"))
		.SetTooltipText(LOCTEXT("TabTooltip", "批量管理材质继承关系，自动检测相同静态开关组合。"))
		.SetMenuType(ETabSpawnerMenuType::Hidden)
		.SetIcon(FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.Tabs.Outliner"));
}

void FMaterialInheritanceManagerModule::UnregisterTabSpawner()
{
	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(MaterialInheritancePanelTabId);
}

TSharedRef<SDockTab> FMaterialInheritanceManagerModule::SpawnMainPanel(const FSpawnTabArgs& Args)
{
	TSharedRef<SDockTab> NewTab = SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		.Label(LOCTEXT("TabTitle", "材质继承管理器"))
		[
			SNew(SMaterialInheritancePanel)
		];

	return NewTab;
}

void FMaterialInheritanceManagerModule::OpenMainPanel()
{
	FGlobalTabmanager::Get()->TryInvokeTab(MaterialInheritancePanelTabId);
}

void FMaterialInheritanceManagerModule::RegisterMenus()
{
	UToolMenus::RegisterStartupCallback(
		FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FMaterialInheritanceManagerModule::RegisterMenus_Internal));
}

void FMaterialInheritanceManagerModule::RegisterMenus_Internal()
{
	FToolMenuOwnerScoped OwnerScoped(this);

	// Shared pipeline and material category.
	{
		UToolMenu* MainMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu");
		FToolMenuSection& Section = MainMenu->FindOrAddSection(NAME_None);
		bool bPipelineExists = false;
		for (FToolMenuEntry& Block : Section.Blocks)
		{
			if (Block.Name == "PipelineTools")
			{
				bPipelineExists = true;
				break;
			}
		}
		if (!bPipelineExists)
		{
			FToolMenuEntry& ToolsEntry = Section.AddSubMenu(
				"PipelineTools",
				LOCTEXT("PipelineToolsMenu", "管线工具"),
				LOCTEXT("PipelineToolsMenu_ToolTip", "管线工具"),
				FNewToolMenuChoice());
			ToolsEntry.InsertPosition = FToolMenuInsert("Window", EToolMenuInsertType::After);
		}
	}
	UToolMenu* PipelineMenu = UToolMenus::Get()->RegisterMenu("LevelEditor.MainMenu.PipelineTools", NAME_None, EMultiBoxType::Menu, false);
	FToolMenuSection& PipelineSection = PipelineMenu->FindOrAddSection(NAME_None);
	bool bMaterialToolsExists = false;
	for (FToolMenuEntry& Block : PipelineSection.Blocks)
	{
		if (Block.Name == "MaterialTools") { bMaterialToolsExists = true; break; }
	}
	if (!bMaterialToolsExists)
	{
		PipelineSection.AddSubMenu("MaterialTools", LOCTEXT("MaterialToolsMenu", "材质工具"),
			LOCTEXT("MaterialToolsTip", "材质工具"), FNewToolMenuChoice());
	}
	UToolMenu* MaterialToolsMenu = UToolMenus::Get()->RegisterMenu(
		"LevelEditor.MainMenu.PipelineTools.MaterialTools", NAME_None, EMultiBoxType::Menu, false);
	{
		FToolMenuSection& Section = MaterialToolsMenu->FindOrAddSection("MaterialInheritanceManager");
		Section.AddMenuEntry(
			"MaterialInheritanceManager_Entry",
			LOCTEXT("MIM_Entry", "材质继承管理器"),
			LOCTEXT("MIM_Entry_Tooltip", "打开材质继承管理器可停靠面板。"),
			FSlateIcon(FMaterialInheritanceManagerStyle::GetStyleSetName(), "MaterialInheritanceManager.OpenPanel"),
			FUIAction(FExecuteAction::CreateRaw(this, &FMaterialInheritanceManagerModule::OpenMainPanel)));
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FMaterialInheritanceManagerModule, MaterialInheritanceManager)
