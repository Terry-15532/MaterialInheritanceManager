#include "MaterialInheritanceTypes.h"

#if WITH_DEV_AUTOMATION_TESTS
#include "AssetRegistry/AssetRegistryModule.h"
#include "HAL/FileManager.h"
#include "Materials/Material.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Misc/AutomationTest.h"
#include "Misc/Guid.h"
#include "Misc/PackageName.h"
#include "UObject/Package.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMaterialInheritanceDeferredSaveTest,
	"EditorTools.MaterialInheritance.DeferredSaveBatch",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FMaterialInheritanceDeferredSaveTest::RunTest(const FString& Parameters)
{
	UMaterial* Source = LoadObject<UMaterial>(nullptr,
		TEXT("/Game/Showcase/MaterialInheritance/M_Showcase_Master.M_Showcase_Master"));
	if (!TestNotNull(TEXT("Showcase material loads"), Source)) return false;
	// AssetTools requires a content mount point; these unique test packages stay in memory only.
	const FString Directory = TEXT("/Game/__Automation/MaterialInheritanceSaveBatch_") + FGuid::NewGuid().ToString(EGuidFormats::Digits);
	TSet<UPackage*> ModifiedPackages;
	TArray<UMaterialInstanceConstant*> CreatedAssets;
	TArray<UMaterialInstanceConstant*> Instances;
	for (int32 Group = 0; Group < 3; ++Group)
	{
		const FString FirstChildName = FString::Printf(TEXT("MI_Group%d"), Group);
		UMaterialInstanceConstant* Shared = CreateOrGetSharedMI(Source, ReadAllEffectiveParams(Source).StaticSwitches,
			Directory, FirstChildName, ModifiedPackages);
		if (!TestNotNull(TEXT("Shared parent created without saving"), Shared)) continue;
		CreatedAssets.Add(Shared);
		TestTrue(TEXT("New parent remains dirty until the final save"), Shared->GetOutermost()->IsDirty());
		TArray<UMaterialInstanceConstant*> Children;
		for (int32 Index = 0; Index < 100; ++Index)
		{
			const FString Name = FString::Printf(TEXT("MI_%d_%d"), Group, Index);
			UPackage* Package = CreatePackage(*(Directory / Name));
			auto* Child = NewObject<UMaterialInstanceConstant>(Package, *Name, RF_Public | RF_Standalone);
			Child->SetParentEditorOnly(Source);
			Child->SetScalarParameterValueEditorOnly(FMaterialParameterInfo(TEXT("Roughness")), 0.1f + Index * 0.005f);
			Children.Add(Child);
			Instances.Add(Child);
		}
		const auto Backup = BackupMaterialParams(Children);
		TestEqual(TEXT("All 100 children reparented"), ReparentInstances(Shared, Children, ModifiedPackages), 100);
		TestEqual(TEXT("One package per new parent and child"), ModifiedPackages.Num(), (Group + 1) * 101);
		const int32 PreviousPackageCount = ModifiedPackages.Num();
		TestEqual(TEXT("Repeated reparent is a no-op"), ReparentInstances(Shared, Children, ModifiedPackages), 0);
		TestEqual(TEXT("No duplicate packages"), ModifiedPackages.Num(), PreviousPackageCount);
		TestEqual(TEXT("Effective parameters survive reparenting"), RestoreMaterialParams(Backup, &ModifiedPackages), 0);
		for (UMaterialInstanceConstant* Child : Children)
		{
			TestEqual(TEXT("Child points to shared parent"), Child->Parent.Get(), static_cast<UMaterialInterface*>(Shared));
			TestTrue(TEXT("Child remains dirty until final save"), Child->GetOutermost()->IsDirty());
		}
	}

	// Repairs can affect instances outside the reparent list: include those packages as well.
	UPackage* RepairPackage = CreatePackage(*(Directory / TEXT("MI_RepairOnly")));
	auto* RepairOnly = NewObject<UMaterialInstanceConstant>(RepairPackage, TEXT("MI_RepairOnly"), RF_Public | RF_Standalone);
	RepairOnly->SetParentEditorOnly(Source);
	RepairOnly->SetScalarParameterValueEditorOnly(FMaterialParameterInfo(TEXT("Roughness")), 0.25f);
	const auto RepairBackup = BackupMaterialParams({RepairOnly});
	RepairOnly->SetScalarParameterValueEditorOnly(FMaterialParameterInfo(TEXT("Roughness")), 0.75f);
	TestEqual(TEXT("Safety-net correction performed"), RestoreMaterialParams(RepairBackup, &ModifiedPackages), 1);
	TestTrue(TEXT("Safety-net-only package included in final save"), ModifiedPackages.Contains(RepairPackage));
	TestTrue(TEXT("Safety-net correction remains dirty until final save"), RepairPackage->IsDirty());
	TestEqual(TEXT("Repaired value is the original value"), ReadAllEffectiveParams(RepairOnly).Scalars.FindRef(TEXT("Roughness")), 0.25f);
	TestFalse(TEXT("Unmodified source excluded from final save"), ModifiedPackages.Contains(Source->GetOutermost()));
	TestEqual(TEXT("300 children, 3 parents, 1 safety-net correction"), ModifiedPackages.Num(), 304);
	for (UPackage* Package : ModifiedPackages)
	{
		const FString Filename = FPackageName::LongPackageNameToFilename(Package->GetName(), FPackageName::GetAssetPackageExtension());
		TestFalse(TEXT("No intermediate asset writes"), IFileManager::Get().FileExists(*Filename));
		Package->SetDirtyFlag(false);
	}
	for (UMaterialInstanceConstant* Asset : CreatedAssets)
	{
		FAssetRegistryModule::AssetDeleted(Asset);
		Asset->ClearFlags(RF_Public | RF_Standalone);
		Asset->MarkAsGarbage();
	}
	for (UMaterialInstanceConstant* Instance : Instances)
	{
		Instance->ClearFlags(RF_Public | RF_Standalone);
		Instance->MarkAsGarbage();
	}
	RepairOnly->ClearFlags(RF_Public | RF_Standalone);
	RepairOnly->MarkAsGarbage();
	return true;
}
#endif
