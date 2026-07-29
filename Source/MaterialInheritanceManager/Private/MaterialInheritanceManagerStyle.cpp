// Copyright Epic Games, Inc. All Rights Reserved.

#include "MaterialInheritanceManagerStyle.h"
#include "Styling/SlateStyleRegistry.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateTypes.h"
#include "Styling/CoreStyle.h"
#include "Styling/AppStyle.h"

#define IMAGE_BRUSH(RelativePath, ...) FSlateImageBrush(StyleInstance->RootToContentDir(RelativePath, TEXT(".png")), __VA_ARGS__)

TSharedPtr<FSlateStyleSet> FMaterialInheritanceManagerStyle::StyleInstance = nullptr;

void FMaterialInheritanceManagerStyle::Initialize()
{
	if (StyleInstance.IsValid())
	{
		return;
	}

	StyleInstance = MakeShareable(new FSlateStyleSet(GetStyleSetName()));

	StyleInstance->SetContentRoot(IPluginManager::Get().FindPlugin("MaterialInheritanceManager")->GetBaseDir() / TEXT("Resources"));

	// No custom brushes needed; use engine default icons via FSlateIcon in menu registration

	FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
}

void FMaterialInheritanceManagerStyle::Shutdown()
{
	if (StyleInstance.IsValid())
	{
		FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
		StyleInstance.Reset();
	}
}

const ISlateStyle& FMaterialInheritanceManagerStyle::Get()
{
	check(StyleInstance.IsValid());
	return *StyleInstance;
}

FName FMaterialInheritanceManagerStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("MaterialInheritanceManagerStyle"));
	return StyleSetName;
}

#undef IMAGE_BRUSH
