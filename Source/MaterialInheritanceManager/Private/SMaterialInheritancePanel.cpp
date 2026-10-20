// Copyright Epic Games, Inc. All Rights Reserved.

#include "SMaterialInheritancePanel.h"
#include "SMaterialInheritanceSidebar.h"
#include "MaterialInheritanceGraph.h"
#include "MaterialInheritanceGraphNode.h"
#include "MaterialInheritanceTypes.h"
#include "MaterialInheritanceIndex.h"
#include "MaterialInheritanceManagerConfig.h"

#include "Materials/Material.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Materials/MaterialExpressionStaticSwitchParameter.h"
#include "Materials/MaterialExpressionConstant3Vector.h"
#include "Materials/MaterialExpressionMultiply.h"
#include "Factories/MaterialInstanceConstantFactoryNew.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "MaterialEditingLibrary.h"
#include "AssetToolsModule.h"
#include "ObjectTools.h"
#include "UObject/Package.h"
#include "UObject/SavePackage.h"
#include "UObject/GarbageCollection.h"
#include "Misc/PackageName.h"
#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"
#include "FileHelpers.h"

#include "GraphEditor.h"
#include "GraphEditorModule.h"
#include "EdGraph/EdGraph.h"
#include "MaterialInheritanceGraphSchema.h"
#include "EdGraph/EdGraphPin.h"
#include "Editor.h"
#include "Subsystems/AssetEditorSubsystem.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SSplitter.h"
#include "Widgets/Images/SImage.h"
#include "EditorStyleSet.h"
#include "Misc/MessageDialog.h"
#include "Misc/ScopedSlowTask.h"
#include "DesktopPlatformModule.h"
#include "Interfaces/IPluginManager.h"
#include "HAL/PlatformProcess.h"
#include "Containers/Ticker.h"
#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#endif

#define LOCTEXT_NAMESPACE "MaterialInheritancePanel"

namespace
{
	constexpr const TCHAR* TestSourceMaterialPath = TEXT("/Game/Showcase/MaterialInheritance/M_Showcase_Master.M_Showcase_Master");
	constexpr const TCHAR* TestParentMaterialName = TEXT("M_Showcase_Master");
	constexpr int32 TestInstanceCount = 1000;
	constexpr int32 TestDirectChildCount = 120;
	const FName TestTintSwitch(TEXT("DemoTint"));
	const FName TestGlossSwitch(TEXT("DemoGlossy"));

	bool AddTestMaterialSwitches(UMaterial* Material)
	{
		if (!Material) return false;
		UMaterialExpression* Color = UMaterialEditingLibrary::GetMaterialPropertyInputNode(Material, MP_BaseColor);
		UMaterialExpression* Roughness = UMaterialEditingLibrary::GetMaterialPropertyInputNode(Material, MP_Roughness);
		if (!Color || !Roughness) return false;
		const FString ColorOutput = UMaterialEditingLibrary::GetMaterialPropertyInputNodeOutputName(Material, MP_BaseColor);
		const FString RoughnessOutput = UMaterialEditingLibrary::GetMaterialPropertyInputNodeOutputName(Material, MP_Roughness);
		auto* Tint = Cast<UMaterialExpressionConstant3Vector>(UMaterialEditingLibrary::CreateMaterialExpression(
			Material, UMaterialExpressionConstant3Vector::StaticClass(), -600, -180));
		auto* Gloss = Cast<UMaterialExpressionMultiply>(UMaterialEditingLibrary::CreateMaterialExpression(
			Material, UMaterialExpressionMultiply::StaticClass(), -360, 300));
		auto* TintSwitch = Cast<UMaterialExpressionStaticSwitchParameter>(UMaterialEditingLibrary::CreateMaterialExpression(
			Material, UMaterialExpressionStaticSwitchParameter::StaticClass(), -120, 0));
		auto* GlossSwitch = Cast<UMaterialExpressionStaticSwitchParameter>(UMaterialEditingLibrary::CreateMaterialExpression(
			Material, UMaterialExpressionStaticSwitchParameter::StaticClass(), -120, 260));
		if (!Tint || !Gloss || !TintSwitch || !GlossSwitch) return false;
		Tint->Constant = FLinearColor(0.8f, 0.18f, 0.05f, 1.0f);
		Gloss->ConstB = 0.35f;
		TintSwitch->ParameterName = TestTintSwitch;
		TintSwitch->DefaultValue = false;
		GlossSwitch->ParameterName = TestGlossSwitch;
		GlossSwitch->DefaultValue = false;
		const bool bConnected =
			UMaterialEditingLibrary::ConnectMaterialExpressions(Tint, TEXT(""), TintSwitch, TEXT("True")) &&
			UMaterialEditingLibrary::ConnectMaterialExpressions(Color, ColorOutput, TintSwitch, TEXT("False")) &&
			UMaterialEditingLibrary::ConnectMaterialExpressions(Roughness, RoughnessOutput, Gloss, TEXT("A")) &&
			UMaterialEditingLibrary::ConnectMaterialExpressions(Gloss, TEXT(""), GlossSwitch, TEXT("True")) &&
			UMaterialEditingLibrary::ConnectMaterialExpressions(Roughness, RoughnessOutput, GlossSwitch, TEXT("False")) &&
			UMaterialEditingLibrary::ConnectMaterialProperty(TintSwitch, TEXT(""), MP_BaseColor) &&
			UMaterialEditingLibrary::ConnectMaterialProperty(GlossSwitch, TEXT(""), MP_Roughness);
		if (!bConnected) return false;
		UMaterialEditingLibrary::RecompileMaterial(Material);
		return true;
	}

	using FAutoMergeGroups = TMap<UMaterialInterface*, TMap<FStaticSwitchSignature, TArray<UMaterialInstanceConstant*>>>;

	FAutoMergeGroups CollectAutoMergeGroups(const TArray<UMaterialInstanceConstant*>& Instances)
	{
		FAutoMergeGroups Groups;
		TMap<UMaterialInterface*, FStaticSwitchSignature> ParentSignatures;
		for (UMaterialInstanceConstant* Instance : Instances)
		{
			if (!Instance || !Instance->Parent) continue;
			UMaterialInterface* Parent = Instance->Parent;
			const FStaticSwitchSignature ChildSig = ReadStaticSwitches(Instance);
			FStaticSwitchSignature* ParentSig = ParentSignatures.Find(Parent);
			if (!ParentSig)
				ParentSig = &ParentSignatures.Add(Parent, ReadAllEffectiveParams(Parent).StaticSwitches);
			if (ChildSig == *ParentSig) continue;
			Groups.FindOrAdd(Parent).FindOrAdd(ChildSig).Add(Instance);
		}
		return Groups;
	}

	UMaterialInstanceConstant* CreateExampleMI(const FString& Name, UMaterialInterface* Parent, float Roughness, const TMap<FName, bool>& Switches, bool bSetRoughness, const FString& OutputDir = TEXT("/Game/Materials/MergeTest"))
	{
		if (!Parent)
		{
			UE_LOG(LogTemp, Error, TEXT("[MIM] 测试实例 %s 缺少母材质。"), *Name);
			return nullptr;
		}
		IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools")).Get();
		UMaterialInstanceConstantFactoryNew* Factory = NewObject<UMaterialInstanceConstantFactoryNew>();
		Factory->InitialParent = Parent;
		UObject* NewObj = AssetTools.CreateAsset(*Name, *OutputDir, UMaterialInstanceConstant::StaticClass(), Factory);
		UMaterialInstanceConstant* MIC = Cast<UMaterialInstanceConstant>(NewObj);
		if (!MIC)
		{
			UE_LOG(LogTemp, Warning, TEXT("[MIM] 创建示例材质失败: %s"), *Name);
			return nullptr;
		}
		for (const auto& P : Switches)
			UMaterialEditingLibrary::SetMaterialInstanceStaticSwitchParameterValue(MIC, P.Key, P.Value,
				EMaterialParameterAssociation::GlobalParameter, false);
		if (bSetRoughness)
			UMaterialEditingLibrary::SetMaterialInstanceScalarParameterValue(MIC, FName(TEXT("Roughness")), Roughness);
		else if (Switches.Num() > 0)
			UMaterialEditingLibrary::UpdateMaterialInstance(MIC);

		UPackage* Pkg = MIC->GetOutermost();
		Pkg->MarkPackageDirty();
		FString Filename = FPackageName::LongPackageNameToFilename(Pkg->GetName(), FPackageName::GetAssetPackageExtension());
		if (!UPackage::SavePackage(Pkg, MIC, *Filename, FSavePackageArgs()))
		{
			UE_LOG(LogTemp, Error, TEXT("[MIM] 保存测试实例失败: %s"), *Filename);
			return nullptr;
		}
		return MIC;
	}

	// 直接生成 1000 个性能测试专用材质，构成较为复杂的树状继承关系。
	// 若目标目录已存在足够数量的材质实例则跳过，避免重复创建。
	void EnsurePerfTestMaterials()
	{
		const FString Dir = TEXT("/Game/Materials/PerfTest");

		IAssetRegistry& AR = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry")).Get();
		FARFilter Filter;
		Filter.bRecursivePaths = true;
		Filter.PackagePaths.Add(FName(*Dir));
		TArray<FAssetData> Existing;
		AR.GetAssets(Filter, Existing);

		const FTopLevelAssetPath MICPath = UMaterialInstanceConstant::StaticClass()->GetClassPathName();
		int32 NumMI = 0;
		for (const FAssetData& A : Existing)
			if (A.AssetClassPath == MICPath) NumMI++;
		if (NumMI >= 1000)
		{
			UE_LOG(LogTemp, Log, TEXT("[MIM] 性能测试材质已存在（%d 个），跳过创建。"), NumMI);
			return;
		}

		UMaterialInterface* Base = LoadObject<UMaterialInterface>(nullptr, TestSourceMaterialPath);
		if (!Base)
		{
			UE_LOG(LogTemp, Warning, TEXT("[MIM] 找不到 %s，无法生成性能测试材质。"), TestSourceMaterialPath);
			return;
		}
		Base->AddToRoot();

		const int32 NumRoots = 8;
		const int32 MaxTotal = 1000;
		const int32 MaxChildren = 3;

		// 先创建若干根材质实例（直接继承自基础材质），形成多个独立子树
		TArray<UMaterialInterface*> Frontier;
		for (int32 r = 0; r < NumRoots; r++)
		{
			FString Name = FString::Printf(TEXT("MI_PerfBase_%d"), r);
			if (UMaterialInstanceConstant* Root = CreateExampleMI(Name, Base, FMath::FRandRange(0.0f, 1.0f), {}, true, Dir))
				Frontier.Add(Root);
		}

		// 在每个子树内按固定分支数层层展开，形成较深的树状继承，直到达到 1000 个
		TMap<UMaterialInterface*, int32> ChildCount;
		int32 Created = Frontier.Num();
		int32 Idx = 0;
		while (Created < MaxTotal && Frontier.Num() > 0)
		{
			UMaterialInterface* Parent = Frontier[Idx % Frontier.Num()];
			Idx++;
			FString Name = FString::Printf(TEXT("MI_Perf_%d"), Created);
			UMaterialInstanceConstant* Child = CreateExampleMI(Name, Parent, FMath::FRandRange(0.0f, 1.0f), {}, true, Dir);
			if (!Child) break;
			Created++;
			int32& N = ChildCount.FindOrAdd(Parent);
			N++;
			if (N >= MaxChildren)
				Frontier.RemoveSingle(Parent);
			Frontier.Add(Child);
		}

		Base->RemoveFromRoot();
		UE_LOG(LogTemp, Log, TEXT("[MIM] 已生成性能测试材质 %d 个，目录：%s"), Created, *Dir);
	}
}

void SMaterialInheritancePanel::Construct(const FArguments& InArgs)
{
	LoadConfig();
	EdGraph = NewObject<UMaterialInheritanceGraph>();
	EdGraph->Schema = UMaterialInheritanceGraphSchema::StaticClass();
	EdGraph->AddToRoot();

	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot().AutoHeight()[ BuildToolbar() ]
		+ SVerticalBox::Slot().AutoHeight().Padding(8,4)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center)
			[ SNew(STextBlock).Text(LOCTEXT("CurMat","当前材质:")).Font(FCoreStyle::GetDefaultFontStyle("Bold",10)).ColorAndOpacity(FLinearColor(0.7f,0.7f,0.7f)) ]
			+ SHorizontalBox::Slot().AutoWidth().Padding(8,0,0,0).VAlign(VAlign_Center)
			[ SNew(STextBlock).Text_Lambda([this](){ if(CurrentMaterial) return FText::FromString(CurrentMaterial->GetPathName()); return LOCTEXT("NoMat","(未选择)"); }).ColorAndOpacity(FLinearColor(0.3f,0.8f,1.0f)).Font(FCoreStyle::GetDefaultFontStyle("Regular",9)) ]
		]
		+ SVerticalBox::Slot().FillHeight(1.0f)
		[
			SNew(SSplitter).Orientation(EOrientation::Orient_Horizontal)
			+ SSplitter::Slot().Value(0.75f)
			[ SNew(SBorder).BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))[ SAssignNew(GraphContainer,SBox).HAlign(HAlign_Fill).VAlign(VAlign_Fill)[ SNew(STextBlock).Text(LOCTEXT("EmptyHint","在内容浏览器中选择材质\n点击 [使用内容浏览器选中项]")).ColorAndOpacity(FLinearColor(0.4f,0.4f,0.4f)).Font(FCoreStyle::GetDefaultFontStyle("Italic",11)).AutoWrapText(true).Justification(ETextJustify::Center) ]] ]
			+ SSplitter::Slot().Value(0.25f).MinSize(200)
			[ SNew(SBorder).BorderImage(FAppStyle::GetBrush("ToolPanel.DarkGroupBorder"))[ SNew(SMaterialInheritanceSidebar).OwnerPanel(this) ] ]
		]
	];
}

SMaterialInheritancePanel::~SMaterialInheritancePanel() { if (EdGraph) EdGraph->RemoveFromRoot(); }

// ============================================================================
TSharedRef<SWidget> SMaterialInheritancePanel::BuildToolbar()
{
	return SNew(SBorder).BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder")).Padding(FMargin(6,4))
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot().AutoWidth().Padding(2,0).VAlign(VAlign_Center)
		[ SNew(SButton).ButtonStyle(FAppStyle::Get(),"SimpleButton").ToolTipText(LOCTEXT("RefreshTip","刷新")).OnClicked(this,&SMaterialInheritancePanel::OnRefresh)[ SNew(SImage).Image(FAppStyle::GetBrush("Icons.Refresh")) ] ]
		+ SHorizontalBox::Slot().AutoWidth().Padding(6,0).VAlign(VAlign_Center)
		[ SNew(STextBlock).Text(FText::FromString("|")).ColorAndOpacity(FLinearColor(0.3f,0.3f,0.3f)) ]
		+ SHorizontalBox::Slot().AutoWidth().Padding(4,0).VAlign(VAlign_Center)
		[ SNew(SButton).Text(LOCTEXT("UseCB","使用内容浏览器选中项")).OnClicked(this,&SMaterialInheritancePanel::OnUseContentBrowserSelection) ]
		+ SHorizontalBox::Slot().FillWidth(1.0f)[ SNullWidget::NullWidget ]
		+ SHorizontalBox::Slot().AutoWidth().Padding(4,0).VAlign(VAlign_Center)
		[
			SNew(SButton)
			.ButtonColorAndOpacity(FLinearColor(0.1f, 0.55f, 0.3f))
			.ForegroundColor(FLinearColor::White)
			.ToolTipText(LOCTEXT("Regen1000Tip","重新生成 1000 个材质实例，用于继承与归并测试"))
			.OnClicked(this, &SMaterialInheritancePanel::OnRegenerateTestAssets)
			[ SNew(STextBlock).Text(LOCTEXT("RegenBtn","重新生成测试用例")).Font(FCoreStyle::GetDefaultFontStyle("Bold", 10)) ]
		]
		+ SHorizontalBox::Slot().AutoWidth().Padding(4,0).VAlign(VAlign_Center)
		[
			SNew(SButton)
			.ButtonColorAndOpacity(FLinearColor(0.12f, 0.45f, 0.9f))
			.ForegroundColor(FLinearColor::White)
			.ToolTipText(LOCTEXT("HelpTip","打开帮助文档"))
			.OnClicked_Lambda([]() -> FReply {
				FString DocPath = IPluginManager::Get().FindPlugin("MaterialInheritanceManager")->GetBaseDir() + "/Docs/Help.html";
				FPlatformProcess::LaunchURL(*DocPath, nullptr, nullptr);
				return FReply::Handled();
			})
			[ SNew(STextBlock).Text(LOCTEXT("HelpBtn","帮助文档")).Font(FCoreStyle::GetDefaultFontStyle("Bold", 10)) ]
		]
		+ SHorizontalBox::Slot().AutoWidth().Padding(4,0).VAlign(VAlign_Center)
		[ SNew(STextBlock).Text(LOCTEXT("OutL","输出:")).ColorAndOpacity(FLinearColor(0.5f,0.5f,0.5f)).Font(FCoreStyle::GetDefaultFontStyle("Regular",9)) ]
		+ SHorizontalBox::Slot().AutoWidth().Padding(2,0).VAlign(VAlign_Center)
		[ SNew(SBox).MinDesiredWidth(160)[ SNew(STextBlock).Text_Lambda([this](){ return FText::FromString(GetOutputDirectory()); }).ColorAndOpacity(FLinearColor(0.7f,0.7f,0.3f)).Font(FCoreStyle::GetDefaultFontStyle("Regular",9)) ] ]
		+ SHorizontalBox::Slot().AutoWidth().Padding(4,0).VAlign(VAlign_Center)
		[ SNew(SButton).ButtonStyle(FAppStyle::Get(),"SimpleButton").OnClicked(this,&SMaterialInheritancePanel::OnBrowseOutputFolder)[ SNew(SImage).Image(FAppStyle::GetBrush("Icons.FolderOpen")) ] ]
	];

	// 直接生成 1000 个性能测试材质（树状继承关系），延迟一次性执行，已存在则跳过
	static bool bPerfTestKicked = false;
	if (!bPerfTestKicked)
	{
		bPerfTestKicked = true;
		FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([](float) -> bool {
			EnsurePerfTestMaterials();
			return false;
		}), 1.0f);
	}
}

void SMaterialInheritancePanel::RefreshGraph()
{
	OnRefresh();
}

FReply SMaterialInheritancePanel::OnRegenerateTestAssets()
{
	const FString MergeTestDir = TEXT("/Game/Materials/MergeTest");
	const FString AutoGenDir = MergeTestDir / TEXT("MI_AutoGenerated");

	// 先清空当前正在查看的图表，释放对 MergeTest 等资产的引用，
	// 避免在删除/覆盖这些文件时因仍被图表持有而导致创建失败。
	CurrentMaterial = nullptr;
	EdGraph->ClearGraph();
	EdGraph->NotifyGraphChanged();
	GraphEditor.Reset();
	if (GraphContainer.IsValid())
		GraphContainer->SetContent(SNew(STextBlock)
			.Text(LOCTEXT("RegenClear","正在重新生成测试用例..."))
			.ColorAndOpacity(FLinearColor(0.4f,0.4f,0.4f))
			.Font(FCoreStyle::GetDefaultFontStyle("Italic",11))
			.AutoWrapText(true)
			.Justification(ETextJustify::Center));

	// 1. 使用项目已有的展示母材质，保留原始展示资产。
	UMaterialInterface* SourceParent = LoadObject<UMaterialInterface>(nullptr, TestSourceMaterialPath);
	if (!SourceParent)
	{
		FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(
			FString::Printf(TEXT("找不到测试母材质：%s"), TestSourceMaterialPath)));
		return FReply::Handled();
	}
	SourceParent->AddToRoot();

	// 2. 清空旧的 MergeTest 测试资产（含母材质与子实例）
	FAssetRegistryModule& ARM = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
	IAssetRegistry& AR = ARM.Get();
	TArray<FAssetData> ToDelete;
	FARFilter Filter;
	Filter.bRecursivePaths = true;
	Filter.PackagePaths.Add(FName(*MergeTestDir));
	Filter.ClassPaths.Add(UMaterialInstanceConstant::StaticClass()->GetClassPathName());
	Filter.ClassPaths.Add(UMaterial::StaticClass()->GetClassPathName());
	AR.GetAssets(Filter, ToDelete);
	if (ToDelete.Num())
	{
		ObjectTools::DeleteAssets(ToDelete, false);
		CollectGarbage(RF_NoFlags, true);
	}

	// 3. 复制并保存到 MergeTest，作为本批测试用例的母材质。
	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	UObject* Dup = AssetTools.DuplicateAsset(TestParentMaterialName, MergeTestDir, SourceParent);
	UMaterialInterface* Parent = Cast<UMaterialInterface>(Dup);
	SourceParent->RemoveFromRoot();
	if (!Parent)
	{
		FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("DupShowcaseFail","复制测试母材质失败，无法生成测试用例。"));
		return FReply::Handled();
	}
	if (!AddTestMaterialSwitches(Cast<UMaterial>(Parent)))
	{
		FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("TestSwitchSetupFail", "配置测试母材质的静态开关失败。"));
		return FReply::Handled();
	}
	UPackage* ParentPackage = Parent->GetOutermost();
	const FString ParentFilename = FPackageName::LongPackageNameToFilename(
		ParentPackage->GetName(), FPackageName::GetAssetPackageExtension());
	if (!UPackage::SavePackage(ParentPackage, Parent, *ParentFilename, FSavePackageArgs()))
	{
		FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("SaveTestParentFail","保存测试母材质失败，无法生成测试用例。"));
		return FReply::Handled();
	}
	Parent->AddToRoot();

	// 4. 解析母材质的静态开关
	FString OutDir = MergeTestDir;
	TArray<FName> SwitchNames;
	UMaterial* BaseMat = Parent->GetMaterial();
	if (BaseMat && BaseMat->IsValidLowLevel())
	{
		UMaterialEditingLibrary::GetStaticSwitchParameterNames(BaseMat, SwitchNames);
	}
	const FName s0 = TestTintSwitch;
	const FName s1 = TestGlossSwitch;
	TArray<FName> ScalarNames;
	UMaterialEditingLibrary::GetScalarParameterNames(Parent, ScalarNames);
	const bool bSetRoughness = ScalarNames.Contains(FName(TEXT("Roughness")));

	int32 CreatedInstanceCount = 0;
	FScopedSlowTask GenerationProgress(TestInstanceCount, LOCTEXT("Generate1000Progress", "正在生成 1000 个测试实例..."));
	GenerationProgress.MakeDialog();
	const auto CreateTestMI = [&](const FString& Name, UMaterialInterface* InstanceParent,
		float Roughness, const TMap<FName, bool>& InstanceSwitches) -> UMaterialInstanceConstant*
	{
		GenerationProgress.EnterProgressFrame(1.0f, FText::FromString(Name));
		UMaterialInstanceConstant* Instance = CreateExampleMI(Name, InstanceParent,
			Roughness, InstanceSwitches, bSetRoughness, OutDir);
		if (Instance) ++CreatedInstanceCount;
		return Instance;
	};

	// 5. 宽扇形（120 个直接子实例，演示大批量归并）
	TArray<TPair<bool, bool>> Combos;
	Combos.Add(TPair<bool, bool>(false, false));
	Combos.Add(TPair<bool, bool>(true, false));
	Combos.Add(TPair<bool, bool>(false, true));
	Combos.Add(TPair<bool, bool>(true, true));
	const int32 InstancesPerCombo = TestDirectChildCount / Combos.Num();
	TMap<FName, bool> Switches;
	TArray<UMaterialInstanceConstant*> AM;
	for (int32 ci = 0; ci < Combos.Num(); ++ci)
	{
		Switches.Empty();
		if (s0 != NAME_None) Switches.Add(s0, Combos[ci].Key);
		if (s1 != NAME_None) Switches.Add(s1, Combos[ci].Value);
		for (int32 k = 0; k < InstancesPerCombo; ++k)
		{
			FString Name = FString::Printf(TEXT("MI_AM_%d%d_%d"), Combos[ci].Key ? 1 : 0, Combos[ci].Value ? 1 : 0, k);
			const float Roughness = 0.05f + ((k + ci * InstancesPerCombo) % 19) * 0.05f;
			UMaterialInstanceConstant* MIC = CreateTestMI(Name, Parent, Roughness, Switches);
			AM.Add(MIC);
		}
	}

	// 6. 深链（层级限制演示）
	UMaterialInstanceConstant* Cur = (AM.Num() > 0) ? AM[0] : nullptr;
	for (int32 d = 2; d <= 6 && Cur; ++d)
		Cur = CreateTestMI(FString::Printf(TEXT("MI_Chain_L%d"), d), Cur, 0.2f + d * 0.1f, TMap<FName, bool>());

	// 7. 灌木树（节点数限制演示）
	UMaterialInstanceConstant* Bush = (AM.Num() > 4) ? AM[4] : nullptr;
	if (Bush)
	{
		for (int32 i = 0; i < 4; ++i)
		{
			UMaterialInstanceConstant* L2 = CreateTestMI(FString::Printf(TEXT("MI_Bush_L2_%d"), i), Bush, 0.30f + i * 0.10f, TMap<FName, bool>());
			if (L2)
				for (int32 j = 0; j < 3; ++j)
					CreateTestMI(FString::Printf(TEXT("MI_Bush_L2_%d_L3_%d"), i, j), L2, 0.10f + j * 0.20f, TMap<FName, bool>());
		}
	}

	// 广度优先扩展四叉分支，补齐到 1000 个，避免堆成一条很长的继承链。
	TArray<UMaterialInstanceConstant*> Frontier;
	Frontier.Reserve(TestInstanceCount);
	for (UMaterialInstanceConstant* Instance : AM)
		if (Instance) Frontier.Add(Instance);
	for (int32 i = 0; CreatedInstanceCount < TestInstanceCount && Frontier.IsValidIndex(i / 4); ++i)
	{
		UMaterialInstanceConstant* BranchParent = Frontier[i / 4];
		TMap<FName, bool> BranchSwitches = ReadStaticSwitches(BranchParent).SwitchValues;
		const FName FlippedSwitch = (i % 4 < 2) ? s0 : s1;
		BranchSwitches.FindOrAdd(FlippedSwitch) = !BranchSwitches.FindRef(FlippedSwitch);
		UMaterialInstanceConstant* Instance = CreateTestMI(
			FString::Printf(TEXT("MI_Perf_%04d"), i), BranchParent,
			0.05f + (i % 19) * 0.05f, BranchSwitches);
		if (!Instance) break;
		Frontier.Add(Instance);
	}

	// 8. 自动归并输出目录指向 MergeTest/MI_AutoGenerated
	SetOutputDirectory(AutoGenDir);
	if (Config)
	{
		Config->MaxNodeCount = 2000;
		Config->SearchDepth = 8;
		SaveConfig();
	}

	// 9. 提示生成位置
	UE_LOG(LogTemp, Log, TEXT("[MIM] 测试实例已生成并保存：%d/%d，目录：%s"), CreatedInstanceCount, TestInstanceCount, *MergeTestDir);
	FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(
		FString::Printf(TEXT("已生成并保存 %d/%d 个测试实例。\n\n母材质：%s/%s\n子实例目录：%s\n自动归并输出目录：%s"), CreatedInstanceCount, TestInstanceCount, *MergeTestDir, TestParentMaterialName, *MergeTestDir, *AutoGenDir)));

	// 10. 延迟 0.5 秒后扫描资产注册表，再查看母材质继承树。
	//     刚写入磁盘的 MI 尚未被 AssetRegistry 索引，必须等其入库后再 ShowGraph，否则图中只显示单个父节点。
	UMaterialInterface* RootParent = Parent;
	FString ScanDir = OutDir;
	FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda(
		[this, RootParent, ScanDir](float) -> bool
		{
			FAssetRegistryModule& ARM2 = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
			IAssetRegistry& AR2 = ARM2.Get();
			AR2.ScanPathsSynchronous({ ScanDir }, /*bForceRescan*/ true);
			AR2.ScanPathsSynchronous({ FPackageName::GetLongPackagePath(RootParent->GetOutermost()->GetName()) }, /*bForceRescan*/ true);
			ShowGraph(RootParent);
			if (RootParent)
				RootParent->RemoveFromRoot();
			return false;
		}), 0.5f);

	return FReply::Handled();
}

// ============================================================================
void SMaterialInheritancePanel::ShowGraph(UMaterialInterface* Mat)
{
	CurrentMaterial = Mat;
	// 查看任意材质时，自动把自动归并的输出目录指向该材质所在目录下的 MI_AutoGenerated。
	if (Mat && Config)
	{
		Config->OutputDirectory = FPackageName::GetLongPackagePath(Mat->GetOutermost()->GetName()) / TEXT("MI_AutoGenerated");
		SaveConfig();
	}
	if (!Mat)
	{
		if (GraphContainer.IsValid())
			GraphContainer->SetContent(SNew(STextBlock).Text(LOCTEXT("EmptyHint2","选择材质后点击\n[使用内容浏览器选中项]")).ColorAndOpacity(FLinearColor(0.4f,0.4f,0.4f)).Font(FCoreStyle::GetDefaultFontStyle("Italic",11)).AutoWrapText(true).Justification(ETextJustify::Center));
		EdGraph->ClearGraph(); EdGraph->NotifyGraphChanged(); GraphEditor.Reset();
		return;
	}
	int32 maxD = Config?Config->SearchDepth:4, maxN = Config?Config->MaxNodeCount:200;
	EdGraph->RebuildGraph(Mat, maxD, maxN);

	FGraphAppearanceInfo App; App.CornerText = LOCTEXT("Corner","材质继承关系图");
	SGraphEditor::FGraphEditorEvents Events;
	Events.OnNodeDoubleClicked.BindRaw(this,&SMaterialInheritancePanel::OnNodeDoubleClicked);
	TSharedRef<SGraphEditor> Ed = SNew(SGraphEditor).GraphToEdit(EdGraph).Appearance(App).GraphEvents(Events).IsEditable(false).ShowGraphStateOverlay(false).AutoExpandActionMenu(false);
	GraphEditor = Ed;
	if (GraphContainer.IsValid()) GraphContainer->SetContent(Ed);
	Ed->ZoomToFit(false);
}

// ============================================================================
// Node actions
// ============================================================================
void SMaterialInheritancePanel::ViewInheritanceOfSelected()
{
	if (UObject* A = GetSelectedAsset()) if (auto* M = Cast<UMaterialInterface>(A)) ShowGraph(M);
}

void SMaterialInheritancePanel::BrowseSelectedToContentBrowser()
{
	if (UObject* A = GetSelectedAsset())
	{
		TArray<UObject*> Objs = {A};
		GEditor->SyncBrowserToObjects(Objs);
	}
}

// ============================================================================
FReply SMaterialInheritancePanel::OnRefresh()
{
	SaveConfig();
	if (CurrentMaterial)
	{
		int32 d=Config?Config->SearchDepth:4, n=Config?Config->MaxNodeCount:200;
		EdGraph->RebuildGraph(CurrentMaterial, d, n);
		if (GraphEditor.IsValid()) GraphEditor->ZoomToFit(false);
	}
	return FReply::Handled();
}

FReply SMaterialInheritancePanel::OnUseContentBrowserSelection()
{
	FContentBrowserModule& CB = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
	TArray<FAssetData> Sel; CB.Get().GetSelectedAssets(Sel);
	for (const FAssetData& A : Sel)
		if (A.GetClass()->IsChildOf(UMaterialInterface::StaticClass()))
			if (UMaterialInterface* Mat = Cast<UMaterialInterface>(A.GetAsset())) { ShowGraph(Mat); return FReply::Handled(); }
	FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("NoMat","请在内容浏览器中选择材质或MI。"));
	return FReply::Handled();
}

FReply SMaterialInheritancePanel::OnBrowseOutputFolder()
{
	IDesktopPlatform* DP = FDesktopPlatformModule::Get(); if(!DP) return FReply::Handled();
	FString FSP;
	if(!FPackageName::TryConvertLongPackageNameToFilename(GetOutputDirectory(),FSP,TEXT(""))) FSP=FPaths::ProjectContentDir();
	FPaths::NormalizeDirectoryName(FSP);
	FString Picked;
	if(DP->OpenDirectoryDialog(FSlateApplication::Get().FindBestParentWindowHandleForDialogs(nullptr),TEXT("选择输出目录"),FSP,Picked))
	{
		if(Picked.StartsWith(FPaths::ProjectContentDir())){
			FString CP = TEXT("/Game")/Picked.RightChop(FPaths::ProjectContentDir().Len()); CP.ReplaceInline(TEXT("\\"),TEXT("/"));
			if(CP.EndsWith(TEXT("/"))) CP=CP.LeftChop(1);
			SetOutputDirectory(CP);
		}else FMessageDialog::Open(EAppMsgType::Ok,LOCTEXT("NotInProj","请选择 Content 目录内的文件夹。"));
	}
	return FReply::Handled();
}

void SMaterialInheritancePanel::OnNodeDoubleClicked(UEdGraphNode* Node)
{
	if (auto* MN = Cast<UMaterialInheritanceGraphNode>(Node))
	{
		if (MN->bIsCollapsedPlaceholder)
		{
			// 展开收起的节点：重建图但保持当前视图位置，不重新聚焦/缩放
			EdGraph->OnExpandCollapsed(MN);
		}
		else if (UMaterialInterface* MA = MN->GetMaterialAsset())
		{
			GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OpenEditorForAsset(MA);
		}
	}
}

int32 SMaterialInheritancePanel::GetSelectedChildCount() const
{
	if(!GraphEditor.IsValid()) return 0; int32 c=0;
	for(UObject* O:GraphEditor->GetSelectedNodes()) if(auto* N=Cast<UMaterialInheritanceGraphNode>(O)) if(!N->bIsBaseMaterial) c++;
	return c;
}

UObject* SMaterialInheritancePanel::GetSelectedAsset() const
{
	if(!GraphEditor.IsValid()) return nullptr;
	for(UObject* O:GraphEditor->GetSelectedNodes()) if(auto* N=Cast<UMaterialInheritanceGraphNode>(O)) if(UMaterialInterface* MA=N->GetMaterialAsset()) return MA;
	return nullptr;
}

void SMaterialInheritancePanel::RenameSelectedAsset(const FString& NewName)
{
	UObject* Asset = GetSelectedAsset();
	if (!Asset || NewName.IsEmpty())
	{
		return;
	}
	if (Asset->GetName() == NewName)
	{
		return; // 名字没变，无需处理
	}

	UPackage* Package = Asset->GetOutermost();
	FString Dir = FPackageName::GetLongPackagePath(Package->GetName());
	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools")).Get();
	TArray<FAssetRenameData> RenameData;
	RenameData.Add(FAssetRenameData(Asset, Dir, NewName));
	// 直接在本界面改名（不弹对话框），并自动更新引用
	AssetTools.RenameAssets(RenameData);

	// 改名成功后，延迟 0.2 秒再刷新继承图（保留画布位置/缩放），并重新选中改名后的节点
	RequestGraphRefresh({ Asset });
}

void SMaterialInheritancePanel::ExecuteCreateSharedParent()
{
	if(!GraphEditor.IsValid()||!CurrentMaterial){FMessageDialog::Open(EAppMsgType::Ok,LOCTEXT("NoGraph","请先加载材质。"));return;}
	TArray<UMaterialInstanceConstant*> MI;
	for(UObject* O:GraphEditor->GetSelectedNodes()){auto* N=Cast<UMaterialInheritanceGraphNode>(O);if(N&&!N->bIsBaseMaterial) if(UMaterialInterface* MA=N->GetMaterialAsset()) if(auto* M=Cast<UMaterialInstanceConstant>(MA)) MI.Add(M);}
	if(MI.Num()<2){FMessageDialog::Open(EAppMsgType::Ok,LOCTEXT("Need2","请勾选 2 个及以上的材质实例（来自同一个母材质）。"));return;}
	// 新共享母材质要挂到这些实例的「直接父级」之下（即原本的材质的母材质），而不是基础 UMaterial。
	UMaterialInterface* P=MI[0]->Parent;
	for(auto* I:MI) if(I->Parent!=P){FMessageDialog::Open(EAppMsgType::Ok,LOCTEXT("DiffP","请只选同一个母材质下的实例。"));return;}
	TArray<FMaterialParamSnapshot> SS; for(auto* I:MI) SS.Add(ReadAllParams(I));
	FMaterialParamSnapshot Mo=CalculateParamModes(SS);
	if(EAppReturnType::Yes!=FMessageDialog::Open(EAppMsgType::YesNo,FText::Format(LOCTEXT("confirm","确定创建共享母材质？"),FText::AsNumber(MI.Num())))) return;
	FSharedParentContext C; C.OriginalParentMaterial=P; C.SourceInstances=MI; C.OutputDirectory=GetOutputDirectory();
	UMaterialInstanceConstant* NewParentMI = nullptr;
if(CreateSharedParentAndReparent(C, NewParentMI))
{
    FMessageDialog::Open(EAppMsgType::Ok,LOCTEXT("SPOK","已创建共享母材质。"));
    // 延迟 0.2 秒刷新继承图（保留画布位置/缩放），并选中新增的共享母材质与被操作的源实例
    TArray<UObject*> SelAssets;
    if(NewParentMI) SelAssets.Add(NewParentMI);
    for(auto* M:MI) if(M) SelAssets.Add(M);
    RequestGraphRefresh(SelAssets);
}
else FMessageDialog::Open(EAppMsgType::Ok,LOCTEXT("SPFail","创建共享母材质失败，请检查输出目录是否可写。"));
}

void SMaterialInheritancePanel::ExecuteChangeParent()
{
    if(!GraphEditor.IsValid()) return;
    TArray<UMaterialInterface*> Sel;
    for(UObject* O:GraphEditor->GetSelectedNodes())
        if(auto* N=Cast<UMaterialInheritanceGraphNode>(O))
            if(UMaterialInterface* MA=N->GetMaterialAsset()) Sel.Add(MA);
    if(Sel.Num()<2)
    {
        FMessageDialog::Open(EAppMsgType::Ok,LOCTEXT("NeedCP","请勾选至少 2 个材质"));
        return;
    }
    UMaterialInterface* NewParent = Sel[0];
    TArray<UMaterialInstanceConstant*> Children;
    for(int32 i=1;i<Sel.Num();++i)
        if(auto* M=Cast<UMaterialInstanceConstant>(Sel[i])) Children.Add(M);
    if(Children.Num()==0)
    {
        FMessageDialog::Open(EAppMsgType::Ok,LOCTEXT("NoMICCP","除指定的母材质外，其余物体均需为材质实例"));
        return;
    }
    if(EAppReturnType::Yes!=FMessageDialog::Open(EAppMsgType::YesNo,
        FText::Format(LOCTEXT("CPConfirm","确定将选中的 {0} 个材质实例的母材质统一改成「{1}」？"),
            FText::AsNumber(Children.Num()), FText::FromString(NewParent->GetName()))))
        return;

    // 操作前备份所有被改母材质的实例的有效参数（含未 override 的）
    auto Backup = BackupMaterialParams(Children);

    TArray<UPackage*> ToSave;
    for(auto* C:Children)
    {
        if(C==NewParent) continue;
        C->SetParentEditorOnly(NewParent);
        C->PostEditChange();
        C->MarkPackageDirty();
        ToSave.AddUnique(C->GetOutermost());
    }
    if(ToSave.Num()) FEditorFileUtils::PromptForCheckoutAndSave(ToSave, false, false);
    RestoreMaterialParams(Backup);
    FMessageDialog::Open(EAppMsgType::Ok, FText::Format(LOCTEXT("CPDone","已将 {0} 个材质实例的母材质改成「{1}」。"), FText::AsNumber(Children.Num()), FText::FromString(NewParent->GetName())));
    RequestGraphRefresh({});
}

void SMaterialInheritancePanel::ExecuteAutoGroupMerge()
{
	if(!CurrentMaterial) return;

	// 递归遍历整棵继承树，收集所有材质实例（不只直接子级）。
	FMaterialInheritanceIndex InheritanceIndex;
	InheritanceIndex.Build();
	TSet<UMaterialInstanceConstant*> AllDescendants;
	TArray<UMaterialInterface*> Queue;
	Queue.Add(CurrentMaterial);
	while(Queue.Num())
	{
		UMaterialInterface* Node = Queue.Pop();
		const TArray<FAssetData> Children = InheritanceIndex.GetChildren(FSoftObjectPath(Node));
		for(const FAssetData& Ad : Children)
		{
			if(UMaterialInstanceConstant* M = Cast<UMaterialInstanceConstant>(Ad.GetAsset()))
			{
				if(AllDescendants.Contains(M)) continue;
				AllDescendants.Add(M);
				Queue.Add(M);
			}
		}
	}

	if(AllDescendants.Num() < 2)
	{
		FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(
			TEXT("整棵继承树中的材质实例少于 2 个，无法自动归并。")));
		return;
	}

	// 按 (直接母材质/实例, 子级静态开关签名) 分组；
	// 仅当子级静态开关与其直接母级不同时才纳入归并（母级开关与子级相同则无合并意义）。
	FAutoMergeGroups Groups = CollectAutoMergeGroups(AllDescendants.Array());

	int32 TotalToMerge = 0, TotalGroups = 0;
	for(auto& Outer : Groups)
		for(auto& Inner : Outer.Value)
			if(Inner.Value.Num() >= 2) { TotalToMerge += Inner.Value.Num(); ++TotalGroups; }

	if(TotalToMerge < 2)
	{
		FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(
			TEXT("没有找到 2 个及以上共享同一直接母材质、且静态开关相同的材质实例，无需归并。")));
		return;
	}

	// 大规模测试只列分组摘要，避免数百个名称把确认框撑出屏幕。
	FString MergeDetail;
	int32 ShownGroups = 0;
	for(auto& Outer : Groups)
	{
		for(auto& Inner : Outer.Value)
		{
			if(Inner.Value.Num() < 2) continue;
			if (ShownGroups >= 6) continue;
			const FString ParentName = Outer.Key ? Outer.Key->GetName() : TEXT("?");
			MergeDetail += FString::Printf(TEXT("\n· %s：%d 个实例（%s）"), *ParentName,
				Inner.Value.Num(), *Inner.Key.ToDisplayString());
			++ShownGroups;
		}
	}
	if (TotalGroups > ShownGroups)
		MergeDetail += FString::Printf(TEXT("\n另有 %d 组。"), TotalGroups - ShownGroups);

	const EAppReturnType::Type Choice = FMessageDialog::Open(EAppMsgType::YesNo,
		FText::Format(LOCTEXT("MergeTreeSummary",
			"将 {0} 个实例归入 {1} 个共享母实例，是否继续？\n{2}"),
			FText::AsNumber(TotalToMerge), FText::AsNumber(TotalGroups), FText::FromString(MergeDetail)));
	if(Choice != EAppReturnType::Yes) return;

	// 操作前备份整棵继承树中所有材质实例的有效参数（含未 override 的，因为母材质会变），
	// 以便操作后强制校验并还原，杜绝任何参数漂移。
	TArray<UMaterialInstanceConstant*> AllDescArray = AllDescendants.Array();
	auto Backup = BackupMaterialParams(AllDescArray);

	// Helpers only collect packages. Save once, after the safety net has restored parameters.
	TSet<UPackage*> ModifiedPackages;
	FScopedSlowTask MergeProgress(TotalGroups + 2, LOCTEXT("AutoMergeProgress", "正在自动归并材质实例…"));
	MergeProgress.MakeDialog();
	int32 Cr = 0, Cn = 0;
	for(auto& Outer : Groups)
	{
		UMaterialInterface* Parent = Outer.Key;
		for(auto& Inner : Outer.Value)
		{
			if(Inner.Value.Num() <= 1) continue;
			MergeProgress.EnterProgressFrame(1, LOCTEXT("AutoMergeGrouping", "正在归并…"));
			const FString FirstName = Inner.Value[0] ? Inner.Value[0]->GetName() : TEXT("");
			if(UMaterialInstanceConstant* Shared = CreateOrGetSharedMI(Parent, Inner.Key, GetOutputDirectory(), FirstName, ModifiedPackages))
			{
				Cr++;
				Cn += ReparentInstances(Shared, Inner.Value, ModifiedPackages);
			}
		}
	}

	// 操作后强制还原所有材质实例的有效参数，确保无漂移
	MergeProgress.EnterProgressFrame(1, LOCTEXT("AutoMergeRestore", "正在校验参数…"));
	const int32 Restored = RestoreMaterialParams(Backup, &ModifiedPackages);
	MergeProgress.EnterProgressFrame(1, LOCTEXT("AutoMergeSave", "正在统一保存修改的资产…"));
	if (!ModifiedPackages.IsEmpty())
	{
		const TArray<UPackage*> PackagesToSave = ModifiedPackages.Array();
		if (FEditorFileUtils::PromptForCheckoutAndSave(PackagesToSave, true, false) != FEditorFileUtils::PR_Success)
		{
			FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("AutoMergeSaveIncomplete",
				"归并已完成，但部分资产未保存。未保存的修改仍保留在编辑器中，请检查权限或版本控制后手动保存。"));
			RequestGraphRefresh({});
			return;
		}
	}

	FMessageDialog::Open(EAppMsgType::Ok, FText::Format(LOCTEXT("MD",
		"已创建 {0} 个共享母材质，归并了 {1} 个材质实例。操作后已强制还原 {2} 个材质实例的参数，确保无漂移。"),
		FText::AsNumber(Cr), FText::AsNumber(Cn), FText::AsNumber(Restored)));
	// 仅在有实际更改时才刷新显示，避免无意义的自动刷新
	if (Cr > 0 || Cn > 0)
		RequestGraphRefresh({});
}

FString SMaterialInheritancePanel::GetOutputDirectory() const
{
	if (Config && !Config->OutputDirectory.IsEmpty())
		return Config->OutputDirectory;
	// 默认输出目录：当前查看材质所在文件夹下的 MI_AutoGenerated
	return GetDefaultOutputDirectory();
}
FString SMaterialInheritancePanel::GetDefaultOutputDirectory() const
{
	// 默认 MI 输出路径 = 当前选中材质目录下的 MI_AutoGenerated 目录
	if (UObject* Sel = GetSelectedAsset())
		if (UMaterialInterface* M = Cast<UMaterialInterface>(Sel))
			return FPackageName::GetLongPackagePath(M->GetOutermost()->GetName()) / TEXT("MI_AutoGenerated");
	if (CurrentMaterial)
		return FPackageName::GetLongPackagePath(CurrentMaterial->GetOutermost()->GetName()) / TEXT("MI_AutoGenerated");
	return TEXT("/Game/Materials/MI_AutoGenerated");
}

void SMaterialInheritancePanel::RequestGraphRefresh(const TArray<UObject*>& SelectAfter)
{
	// 所有修改类操作完成后，延迟 0.2 秒再刷新继承图。
	// 直接重建图（RebuildGraph 内部会刷新），因此保留当前画布的位置与缩放，
	// 操作结束后不突兀地跳回原点。
	TArray<UObject*> Sel = SelectAfter;
	FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda(
		[this, Sel](float) -> bool
		{
			if (CurrentMaterial && Config && EdGraph)
			{
				EdGraph->RebuildGraph(CurrentMaterial, Config->SearchDepth, Config->MaxNodeCount);
				for (UObject* A : Sel)
					if (A) SelectNodesByAssets({ A });
			}
			return false;
		}), 0.2f);
}
void SMaterialInheritancePanel::SetOutputDirectory(const FString& D){if(Config){Config->OutputDirectory=D;SaveConfig();}}
int32 SMaterialInheritancePanel::GetSearchDepth() const{return Config?Config->SearchDepth:4;}
void SMaterialInheritancePanel::SetSearchDepth(int32 V){if(Config){Config->SearchDepth=V;SaveConfig();}RefreshWithoutZoom();}
int32 SMaterialInheritancePanel::GetMaxNodeCount() const{return Config?Config->MaxNodeCount:200;}
void SMaterialInheritancePanel::SetMaxNodeCount(int32 V){if(Config){Config->MaxNodeCount=V;SaveConfig();}RefreshWithoutZoom();}
void SMaterialInheritancePanel::SetSearchDepthLive(int32 V){if(Config)Config->SearchDepth=V;RefreshWithoutZoom();}
void SMaterialInheritancePanel::SetMaxNodeCountLive(int32 V){if(Config)Config->MaxNodeCount=V;RefreshWithoutZoom();}
void SMaterialInheritancePanel::RefreshWithoutZoom()
{
	if(CurrentMaterial && Config && EdGraph)
		EdGraph->RebuildGraph(CurrentMaterial, Config->SearchDepth, Config->MaxNodeCount);
}
void SMaterialInheritancePanel::SelectNodesByAssets(const TArray<UObject*>& Assets)
{
	if(!GraphEditor.IsValid()) return;
	GraphEditor->ClearSelectionSet();
	for(UEdGraphNode* N : EdGraph->Nodes)
	{
		if(auto* MN=Cast<UMaterialInheritanceGraphNode>(N))
			if(UMaterialInterface* MA=MN->GetMaterialAsset())
				if(Assets.Contains(MA))
				GraphEditor->SetNodeSelection(MN, true);
	}
}
void SMaterialInheritancePanel::LoadConfig(){Config=GetMutableDefault<UMaterialInheritanceManagerConfig>();if(Config)Config->LoadConfig();}
void SMaterialInheritancePanel::SaveConfig(){if(Config)Config->SaveConfig();}

#if WITH_DEV_AUTOMATION_TESTS
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMaterialInheritanceAutoMergeShowcaseTest,
	"EditorTools.MaterialInheritance.AutoMergeShowcase",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FMaterialInheritanceAutoMergeShowcaseTest::RunTest(const FString& Parameters)
{
	UMaterial* Source = LoadObject<UMaterial>(nullptr, TestSourceMaterialPath);
	if (!TestNotNull(TEXT("Project showcase source loads"), Source)) return false;
	UMaterial* Material = DuplicateObject<UMaterial>(Source, GetTransientPackage());
	Material->SetFlags(RF_Transient);
	if (!TestTrue(TEXT("Test switches connect to material outputs"), AddTestMaterialSwitches(Material))) return false;
	TArray<UMaterialInstanceConstant*> Instances;
	const auto NewInstance = [&](UMaterialInterface* Parent, bool bTint, bool bGloss, float Roughness)
	{
		auto* Instance = NewObject<UMaterialInstanceConstant>(GetTransientPackage());
		Instance->SetParentEditorOnly(Parent);
		Instance->SetStaticSwitchParameterValueEditorOnly(FMaterialParameterInfo(TestTintSwitch), bTint);
		Instance->SetStaticSwitchParameterValueEditorOnly(FMaterialParameterInfo(TestGlossSwitch), bGloss);
		Instance->SetScalarParameterValueEditorOnly(FMaterialParameterInfo(TEXT("Roughness")), Roughness);
		UMaterialEditingLibrary::UpdateMaterialInstance(Instance);
		return Instance;
	};
	for (int32 Combo = 0; Combo < 4; ++Combo)
		for (int32 Sibling = 0; Sibling < 2; ++Sibling)
			Instances.Add(NewInstance(Material, (Combo & 1) != 0, (Combo & 2) != 0, 0.1f + Combo * 0.15f + Sibling * 0.05f));
	Instances.Add(NewInstance(Instances[2], false, false, 0.3f));
	Instances.Add(NewInstance(Instances[2], false, false, 0.4f));
	Instances.Add(NewInstance(Instances[4], true, true, 0.6f));
	Instances.Add(NewInstance(Instances[4], true, true, 0.7f));

	FAutoMergeGroups Groups = CollectAutoMergeGroups(Instances);
	int32 GroupCount = 0, MergeCount = 0;
	for (const auto& Parents : Groups)
		for (const auto& Group : Parents.Value)
			if (Group.Value.Num() >= 2) { ++GroupCount; MergeCount += Group.Value.Num(); }
	TestEqual(TEXT("Three root groups plus two nested groups"), GroupCount, 5);
	TestEqual(TEXT("Default controls excluded"), MergeCount, 10);
	TArray<UMaterialInstanceConstant*> SharedInstances;
	for (const auto& Parents : Groups)
	{
		for (const auto& Group : Parents.Value)
		{
			if (Group.Value.Num() < 2) continue;
			auto* Shared = NewInstance(Parents.Key, Group.Key.SwitchValues.FindRef(TestTintSwitch),
				Group.Key.SwitchValues.FindRef(TestGlossSwitch), 0.42f);
			SharedInstances.Add(Shared);
			for (UMaterialInstanceConstant* Child : Group.Value)
			{
				const FMaterialParamSnapshot Before = ReadAllEffectiveParams(Child);
				UMaterialEditingLibrary::SetMaterialInstanceParent(Child, Shared);
				UMaterialEditingLibrary::UpdateMaterialInstance(Child);
				const FMaterialParamSnapshot After = ReadAllEffectiveParams(Child);
				TestTrue(TEXT("Static switch values survive regrouping"), Before.StaticSwitches == After.StaticSwitches);
				TestEqual(TEXT("Roughness survives regrouping"), After.Scalars.FindRef(TEXT("Roughness")), Before.Scalars.FindRef(TEXT("Roughness")));
				TestTrue(TEXT("BaseTint survives regrouping"), After.Vectors.FindRef(TEXT("BaseTint")).Equals(Before.Vectors.FindRef(TEXT("BaseTint"))));
			}
		}
	}
	Instances.Append(SharedInstances);
	int32 SecondMergeCount = 0;
	for (const auto& Parents : CollectAutoMergeGroups(Instances))
		for (const auto& Group : Parents.Value)
			if (Group.Value.Num() >= 2) SecondMergeCount += Group.Value.Num();
	TestEqual(TEXT("Second merge finds no eligible groups"), SecondMergeCount, 0);
	return true;
}
#endif

#undef LOCTEXT_NAMESPACE
