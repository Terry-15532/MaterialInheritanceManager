#include "MaterialInheritanceGraph.h"
#include "MaterialInheritanceGraphNode.h"
#include "MaterialInheritanceGraphSchema.h"
#include "MaterialInheritanceIndex.h"
#include "MaterialInheritanceTypes.h"

#if WITH_DEV_AUTOMATION_TESTS
#include "AssetRegistry/AssetRegistryModule.h"
#include "Materials/Material.h"
#include "Materials/MaterialInstanceConstant.h"
#include "MaterialEditingLibrary.h"
#include "Misc/AutomationTest.h"
#include "Misc/Guid.h"
#include "UObject/Package.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMaterialInheritanceRenameTest,
	"EditorTools.MaterialInheritance.RenameHierarchy",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FMaterialInheritanceRenameTest::RunTest(const FString& Parameters)
{
	UMaterial* Source = LoadObject<UMaterial>(nullptr,
		TEXT("/Game/Showcase/MaterialInheritance/M_Showcase_Master.M_Showcase_Master"));
	if (!TestNotNull(TEXT("Source loads"), Source)) return false;
	const FString Directory = TEXT("/Game/__Automation/MIRename_") + FGuid::NewGuid().ToString(EGuidFormats::Digits);
	TArray<UMaterialInstanceConstant*> Created;
	const auto MakeInstance = [&](const TCHAR* PackageName, const TCHAR* ObjectName, UMaterialInterface* Parent)
	{
		auto* Instance = NewObject<UMaterialInstanceConstant>(CreatePackage(*(Directory / PackageName)), ObjectName, RF_Public | RF_Standalone);
		Instance->SetParentEditorOnly(Parent);
		FAssetRegistryModule::AssetCreated(Instance);
		Created.Add(Instance);
		return Instance;
	};
	auto* Parent = MakeInstance(TEXT("ParentPackage"), TEXT("ParentOriginal"), Source);
	auto* Child = MakeInstance(TEXT("ChildPackage"), TEXT("ChildOriginal"), Parent);
	auto* Sibling = MakeInstance(TEXT("SiblingPackage"), TEXT("Sibling"), Parent);
	auto* Grandchild = MakeInstance(TEXT("GrandchildPackage"), TEXT("Grandchild"), Child);
	auto* Graph = NewObject<UMaterialInheritanceGraph>();
	Graph->Schema = UMaterialInheritanceGraphSchema::StaticClass();
	const auto CheckHierarchy = [&]()
	{
		FMaterialInheritanceIndex Index;
		Index.Build();
		TestEqual(TEXT("Exactly two direct children, not all referencers"), Index.GetChildren(FSoftObjectPath(Parent)).Num(), 2);
		TestEqual(TEXT("Grandchild found"), Index.GetChildren(FSoftObjectPath(Child)).Num(), 1);
		TestEqual(TEXT("Actual direct parent"), Index.GetParent(FSoftObjectPath(Child)).GetSoftObjectPath(), FSoftObjectPath(Parent));
		TestEqual(TEXT("Actual grandchild parent"), Index.GetParent(FSoftObjectPath(Grandchild)).GetSoftObjectPath(), FSoftObjectPath(Child));
		Graph->RebuildGraph(Parent, 4, 100);
		for (UMaterialInstanceConstant* Instance : Created)
		{
			int32 Found = 0;
			for (UEdGraphNode* Node : Graph->Nodes)
				if (auto* MaterialNode = Cast<UMaterialInheritanceGraphNode>(Node))
					if (MaterialNode->AssetPath == FSoftObjectPath(Instance)) ++Found;
			TestEqual(TEXT("Each material appears once after graph rebuild"), Found, 1);
		}
	};
	CheckHierarchy();
	const FString OldParentPath = Parent->GetPathName();
	TestTrue(TEXT("Rename parent without resaving children"), Parent->Rename(TEXT("ParentRenamed"), nullptr, REN_DontCreateRedirectors | REN_NonTransactional));
	FAssetRegistryModule::AssetRenamed(Parent, OldParentPath);
	CheckHierarchy();
	const FString OldChildPath = Child->GetPathName();
	TestTrue(TEXT("Move and rename intermediate instance"), Child->Rename(TEXT("ChildRenamed"),
		CreatePackage(*(Directory / TEXT("MovedChild"))), REN_DontCreateRedirectors | REN_NonTransactional));
	FAssetRegistryModule::AssetRenamed(Child, OldChildPath);
	CheckHierarchy();
	for (UMaterialInstanceConstant* Instance : Created)
	{
		FAssetRegistryModule::AssetDeleted(Instance);
		Instance->GetOutermost()->SetDirtyFlag(false);
		Instance->ClearFlags(RF_Public | RF_Standalone);
		Instance->MarkAsGarbage();
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMaterialInheritanceOverrideVerificationTest,
	"EditorTools.MaterialInheritance.OverrideVerification",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FMaterialInheritanceOverrideVerificationTest::RunTest(const FString& Parameters)
{
	UMaterial* Source = LoadObject<UMaterial>(nullptr,
		TEXT("/Game/Showcase/MaterialInheritance/M_Showcase_Master.M_Showcase_Master"));
	if (!TestNotNull(TEXT("Source loads"), Source)) return false;
	auto* Parent = NewObject<UMaterialInstanceConstant>(GetTransientPackage());
	Parent->SetParentEditorOnly(Source);
	Parent->SetScalarParameterValueEditorOnly(FMaterialParameterInfo(TEXT("Roughness")), 0.4f);
	auto* Child = NewObject<UMaterialInstanceConstant>(GetTransientPackage());
	Child->SetParentEditorOnly(Parent);
	const FMaterialParamSnapshot Before = ReadMaterialVerificationSnapshot(Child);
	Child->SetScalarParameterValueEditorOnly(FMaterialParameterInfo(TEXT("Roughness")), 0.4f);
	const FMaterialParamSnapshot After = ReadMaterialVerificationSnapshot(Child);
	TestEqual(TEXT("Effective value stays the same"), Before.Scalars.FindRef(TEXT("Roughness")), After.Scalars.FindRef(TEXT("Roughness")));
	FString Diff;
	TestTrue(TEXT("Checking override with equal value is a change"), CompareMaterialOverrideStates(Before, After, Diff));
	TestTrue(TEXT("Log names affected parameter"), Diff.Contains(TEXT("Roughness")));
	TestTrue(TEXT("Log shows checkbox direction"), Diff.Contains(TEXT("未勾选 -> 勾选")));
	Child->ScalarParameterValues.Empty();
	Diff.Empty();
	TestTrue(TEXT("Unchecking equal-value override is also a change"), CompareMaterialOverrideStates(After, ReadMaterialVerificationSnapshot(Child), Diff));
	TestTrue(TEXT("Log shows reverse checkbox direction"), Diff.Contains(TEXT("勾选 -> 未勾选")));
	Diff.Empty();
	TestFalse(TEXT("Inherited parent override is not a local checkbox"), CompareMaterialOverrideStates(Before, ReadMaterialVerificationSnapshot(Child), Diff));

	// Local static override metadata must also be distinct from its resolved value.
	const FName SwitchName(TEXT("DemoTint"));
	const bool Inherited = ReadAllEffectiveParams(Child).StaticSwitches.SwitchValues.FindRef(SwitchName);
	const FMaterialParamSnapshot BeforeSwitch = ReadMaterialVerificationSnapshot(Child);
	Child->SetStaticSwitchParameterValueEditorOnly(FMaterialParameterInfo(SwitchName), Inherited);
	UMaterialEditingLibrary::UpdateMaterialInstance(Child);
	const FMaterialParamSnapshot AfterSwitch = ReadMaterialVerificationSnapshot(Child);
	Diff.Empty();
	TestTrue(TEXT("Equal-value static override is detected"), CompareMaterialOverrideStates(BeforeSwitch, AfterSwitch, Diff));
	TestTrue(TEXT("Static override log identifies switch"), Diff.Contains(TEXT("DemoTint")));
	TestTrue(TEXT("Static switch effective values stay equal"), BeforeSwitch.StaticSwitches == AfterSwitch.StaticSwitches);

	// Layer/blend parameters sharing a name must not overwrite each other's checkbox states.
	FScalarParameterValue Layer;
	Layer.ParameterInfo = FMaterialParameterInfo(TEXT("SameName"), EMaterialParameterAssociation::LayerParameter, 0);
	Layer.ParameterValue = 0.5f;
	Child->ScalarParameterValues.Add(Layer);
	const FMaterialParamSnapshot BeforeLayer = ReadMaterialVerificationSnapshot(Child);
	Child->ScalarParameterValues.Last().ParameterInfo.Index = 1;
	Diff.Empty();
	TestTrue(TEXT("Layer identities remain distinct"), CompareMaterialOverrideStates(BeforeLayer, ReadMaterialVerificationSnapshot(Child), Diff));
	TestTrue(TEXT("Log includes old layer index"), Diff.Contains(TEXT("索引=0")));
	TestTrue(TEXT("Log includes new layer index"), Diff.Contains(TEXT("索引=1")));
	return true;
}
#endif
