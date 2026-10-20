// Copyright Epic Games, Inc. All Rights Reserved.

#include "MaterialInheritanceTypes.h"

#include "Materials/Material.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Materials/MaterialInstance.h"
#include "Factories/MaterialInstanceConstantFactoryNew.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetToolsModule.h"
#include "IAssetTools.h"
#include "ObjectTools.h"
#include "Misc/MessageDialog.h"
#include "Misc/PackageName.h"
#include "Editor.h"
#include "FileHelpers.h"
#include "UObject/Package.h"
#include "MaterialEditingLibrary.h"

#define LOCTEXT_NAMESPACE "MaterialInheritanceTypes"

// ============================================================================
// FStaticSwitchSignature
// ============================================================================

bool FStaticSwitchSignature::operator==(const FStaticSwitchSignature& Other) const
{
	if (SwitchValues.Num() != Other.SwitchValues.Num()) return false;
	for (const auto& Pair : SwitchValues)
	{
		const bool* OV = Other.SwitchValues.Find(Pair.Key);
		if (!OV || *OV != Pair.Value) return false;
	}
	return true;
}

FString FStaticSwitchSignature::ToDisplayString() const
{
	if (SwitchValues.Num() == 0) return TEXT("(none)");
	TArray<FString> Parts;
	for (const auto& Pair : SwitchValues)
	{
		FString Short = Pair.Key.ToString();
		if (Short.Len() > 6) Short = Short.Left(6);
		Parts.Add(FString::Printf(TEXT("%s=%d"), *Short, Pair.Value ? 1 : 0));
	}
	return FString::Join(Parts, TEXT(", "));
}

// ============================================================================
// FMaterialParamSnapshot
// ============================================================================

bool FMaterialParamSnapshot::IsEmpty() const
{
	return Scalars.Num() == 0 && Vectors.Num() == 0 && Textures.Num() == 0 && StaticSwitches.IsEmpty();
}

FString FMaterialParamSnapshot::ToDisplayString() const
{
	TArray<FString> Lines;
	for (const auto& P : Scalars)
		Lines.Add(FString::Printf(TEXT("  %s = %.3f"), *P.Key.ToString(), P.Value));
	for (const auto& P : Vectors)
		Lines.Add(FString::Printf(TEXT("  %s = (%.2f, %.2f, %.2f)"), *P.Key.ToString(), P.Value.R, P.Value.G, P.Value.B));
	for (const auto& P : Textures)
		Lines.Add(FString::Printf(TEXT("  %s = %s"), *P.Key.ToString(), P.Value.IsValid() ? *P.Value.GetAssetName() : TEXT("(none)")));
	Lines.Add(FString::Printf(TEXT("  Switches: %s"), *StaticSwitches.ToDisplayString()));
	return FString::Join(Lines, TEXT("\n"));
}

// ============================================================================
// Read All Params
// ============================================================================

FMaterialParamSnapshot ReadAllParams(UMaterialInstanceConstant* MI)
{
	FMaterialParamSnapshot Snap;
	if (!MI) return Snap;

	// Scalars
	for (const FScalarParameterValue& Val : MI->ScalarParameterValues)
		Snap.Scalars.Add(Val.ParameterInfo.Name, Val.ParameterValue);

	// Vectors
	for (const FVectorParameterValue& Val : MI->VectorParameterValues)
		Snap.Vectors.Add(Val.ParameterInfo.Name, Val.ParameterValue);

	// Textures
	for (const FTextureParameterValue& Val : MI->TextureParameterValues)
		Snap.Textures.Add(Val.ParameterInfo.Name, Val.ParameterValue);

	// Static Switches
	Snap.StaticSwitches = ReadStaticSwitches(MI);

	return Snap;
}

// ============================================================================
// Read All Effective Params (inheritance-resolved)
// ============================================================================

FMaterialParamSnapshot ReadAllEffectiveParams(UMaterialInterface* Mat)
{
	FMaterialParamSnapshot Snap;
	if (!Mat) return Snap;

	// Scalars
	{
		TArray<FMaterialParameterInfo> Infos;
		TArray<FGuid> Ids;
		Mat->GetAllScalarParameterInfo(Infos, Ids);
		for (const FMaterialParameterInfo& Info : Infos)
		{
			float V = 0.f;
			if (Mat->GetScalarParameterValue(FHashedMaterialParameterInfo(Info), V))
				Snap.Scalars.Add(Info.Name, V);
		}
	}

	// Vectors
	{
		TArray<FMaterialParameterInfo> Infos;
		TArray<FGuid> Ids;
		Mat->GetAllVectorParameterInfo(Infos, Ids);
		for (const FMaterialParameterInfo& Info : Infos)
		{
			FLinearColor V = FLinearColor::Black;
			if (Mat->GetVectorParameterValue(FHashedMaterialParameterInfo(Info), V))
				Snap.Vectors.Add(Info.Name, V);
		}
	}

	// Textures
	{
		TArray<FMaterialParameterInfo> Infos;
		TArray<FGuid> Ids;
		Mat->GetAllTextureParameterInfo(Infos, Ids);
		for (const FMaterialParameterInfo& Info : Infos)
		{
			UTexture* V = nullptr;
			if (Mat->GetTextureParameterValue(FHashedMaterialParameterInfo(Info), V))
				Snap.Textures.Add(Info.Name, V);
		}
	}

	// Static Switches (effective, inheritance-resolved)
	{
		TMap<FMaterialParameterInfo, FMaterialParameterMetadata> Params;
		Mat->GetAllParametersOfType(EMaterialParameterType::StaticSwitch, Params);
		for (const auto& Pair : Params)
		{
			bool V = false;
			FGuid Guid;
			if (Mat->GetStaticSwitchParameterValue(FHashedMaterialParameterInfo(Pair.Key), V, Guid))
				Snap.StaticSwitches.SwitchValues.Add(Pair.Key.Name, V);
		}
	}

	return Snap;
}

// ============================================================================
// Read Static Switches
// ============================================================================

FMaterialParamSnapshot ReadMaterialVerificationSnapshot(UMaterialInterface* Mat)
{
	FMaterialParamSnapshot Snapshot = ReadAllEffectiveParams(Mat);
	if (!Mat) return Snapshot;
	const auto Key = [](EMaterialParameterType Type, const FMaterialParameterInfo& Info)
	{
		const TCHAR* Labels[] = { TEXT("标量"), TEXT("向量"), TEXT("双精度向量"), TEXT("纹理"),
			TEXT("纹理集合"), TEXT("字体"), TEXT("虚拟纹理"), TEXT("稀疏体积纹理"),
			TEXT("静态开关"), TEXT("参数集合"), TEXT("静态通道遮罩") };
		static_assert(UE_ARRAY_COUNT(Labels) == static_cast<int32>(EMaterialParameterType::Num));
		FString Result = FString::Printf(TEXT("%s %s"), Labels[static_cast<int32>(Type)], *Info.Name.ToString());
		if (Info.Association != EMaterialParameterAssociation::GlobalParameter)
			Result += FString::Printf(TEXT(" [关联=%d, 索引=%d]"), static_cast<int32>(Info.Association), Info.Index);
		return Result;
	};
	for (int32 Index = 0; Index < static_cast<int32>(EMaterialParameterType::Num); ++Index)
	{
		const EMaterialParameterType Type = static_cast<EMaterialParameterType>(Index);
		TMap<FMaterialParameterInfo, FMaterialParameterMetadata> Parameters;
		Mat->GetAllParametersOfType(Type, Parameters);
		for (const auto& Pair : Parameters) Snapshot.OverrideStates.Add(Key(Type, Pair.Key), false);
	}
	if (UMaterialInstanceConstant* Instance = Cast<UMaterialInstanceConstant>(Mat))
	{
		// Inspect local arrays, not inherited metadata. Null texture overrides and
		// overrides equal to the parent value still have a checked checkbox.
		const auto Add = [&](EMaterialParameterType Type, const auto& Values)
		{
			for (const auto& Value : Values)
				Snapshot.OverrideStates.Add(Key(Type, Value.ParameterInfo), Value.IsOverride());
		};
		Add(EMaterialParameterType::Scalar, Instance->ScalarParameterValues);
		Add(EMaterialParameterType::Vector, Instance->VectorParameterValues);
		Add(EMaterialParameterType::DoubleVector, Instance->DoubleVectorParameterValues);
		Add(EMaterialParameterType::Texture, Instance->TextureParameterValues);
		Add(EMaterialParameterType::TextureCollection, Instance->TextureCollectionParameterValues);
		Add(EMaterialParameterType::ParameterCollection, Instance->ParameterCollectionParameterValues);
		Add(EMaterialParameterType::RuntimeVirtualTexture, Instance->RuntimeVirtualTextureParameterValues);
		Add(EMaterialParameterType::SparseVolumeTexture, Instance->SparseVolumeTextureParameterValues);
		Add(EMaterialParameterType::Font, Instance->FontParameterValues);
		const FStaticParameterSet StaticParameters = Instance->GetStaticParameters();
		Add(EMaterialParameterType::StaticSwitch, StaticParameters.StaticSwitchParameters);
		Add(EMaterialParameterType::StaticComponentMask, StaticParameters.EditorOnly.StaticComponentMaskParameters);
	}
	return Snapshot;
}

bool CompareMaterialOverrideStates(const FMaterialParamSnapshot& Before,
	const FMaterialParamSnapshot& After, FString& OutDiff)
{
	TSet<FString> Keys;
	for (const auto& Pair : Before.OverrideStates) Keys.Add(Pair.Key);
	for (const auto& Pair : After.OverrideStates) Keys.Add(Pair.Key);
	TArray<FString> SortedKeys = Keys.Array();
	SortedKeys.Sort();
	bool bChanged = false;
	for (const FString& Key : SortedKeys)
	{
		const bool bOld = Before.OverrideStates.FindRef(Key);
		const bool bNew = After.OverrideStates.FindRef(Key);
		if (bOld != bNew)
		{
			bChanged = true;
			OutDiff += FString::Printf(TEXT("\n    %s 覆写: %s -> %s"), *Key,
				bOld ? TEXT("勾选") : TEXT("未勾选"), bNew ? TEXT("勾选") : TEXT("未勾选"));
		}
	}
	return bChanged;
}

FStaticSwitchSignature ReadStaticSwitches(UMaterialInstanceConstant* MI)
{
	FStaticSwitchSignature Sig;
	if (!MI) return Sig;

	FStaticParameterSet Params;
	MI->GetStaticParameterValues(Params);
	for (const FStaticSwitchParameter& S : Params.StaticSwitchParameters)
		Sig.SwitchValues.Add(S.ParameterInfo.Name, S.Value);

	return Sig;
}

// ============================================================================
// Calculate Parameter Modes
// ============================================================================

FMaterialParamSnapshot CalculateParamModes(const TArray<FMaterialParamSnapshot>& Snapshots)
{
	FMaterialParamSnapshot Modes;
	if (Snapshots.Num() == 0) return Modes;

	// --- Scalars: find most frequent value per parameter name ---
	{
		TMap<FName, TArray<float>> AllValues;
		for (const FMaterialParamSnapshot& S : Snapshots)
			for (const auto& P : S.Scalars)
				AllValues.FindOrAdd(P.Key).Add(P.Value);

		for (auto& Pair : AllValues)
		{
			TMap<float, int32> Freq;
			for (float V : Pair.Value) Freq.FindOrAdd(V, 0)++;
			float BestVal = Pair.Value[0];
			int32 BestCnt = 0;
			for (const auto& F : Freq)
			{
				if (F.Value > BestCnt) { BestCnt = F.Value; BestVal = F.Key; }
			}
			Modes.Scalars.Add(Pair.Key, BestVal);
		}
	}

	// --- Vectors: find most frequent color per parameter name ---
	{
		TMap<FName, TArray<FLinearColor>> AllValues;
		for (const FMaterialParamSnapshot& S : Snapshots)
			for (const auto& P : S.Vectors)
				AllValues.FindOrAdd(P.Key).Add(P.Value);

		for (auto& Pair : AllValues)
		{
			TMap<FLinearColor, int32> Freq;
			for (const FLinearColor& V : Pair.Value) Freq.FindOrAdd(V, 0)++;
			FLinearColor BestVal = Pair.Value[0];
			int32 BestCnt = 0;
			for (const auto& F : Freq)
			{
				if (F.Value > BestCnt) { BestCnt = F.Value; BestVal = F.Key; }
			}
			Modes.Vectors.Add(Pair.Key, BestVal);
		}
	}

	// --- Textures: find most frequent texture per parameter name ---
	{
		TMap<FName, TArray<TSoftObjectPtr<UTexture>>> AllValues;
		for (const FMaterialParamSnapshot& S : Snapshots)
			for (const auto& P : S.Textures)
				AllValues.FindOrAdd(P.Key).Add(P.Value);

		for (auto& Pair : AllValues)
		{
			TMap<TSoftObjectPtr<UTexture>, int32> Freq;
			for (const auto& V : Pair.Value) Freq.FindOrAdd(V, 0)++;
			TSoftObjectPtr<UTexture> BestVal = Pair.Value[0];
			int32 BestCnt = 0;
			for (const auto& F : Freq)
			{
				if (F.Value > BestCnt) { BestCnt = F.Value; BestVal = F.Key; }
			}
			Modes.Textures.Add(Pair.Key, BestVal);
		}
	}

	// --- Static Switches: mode per parameter ---
	{
		TMap<FName, TArray<bool>> AllValues;
		for (const FMaterialParamSnapshot& S : Snapshots)
			for (const auto& P : S.StaticSwitches.SwitchValues)
				AllValues.FindOrAdd(P.Key).Add(P.Value);

		for (auto& Pair : AllValues)
		{
			int32 TrueCnt = 0, FalseCnt = 0;
			for (bool V : Pair.Value) { if (V) TrueCnt++; else FalseCnt++; }
			Modes.StaticSwitches.SwitchValues.Add(Pair.Key, TrueCnt >= FalseCnt);
		}
	}

	return Modes;
}

// ============================================================================
// Create Shared Parent & Reparent
// ============================================================================

bool CreateSharedParentAndReparent(const FSharedParentContext& Context, UMaterialInstanceConstant*& OutCreatedSharedParent)
{
	if (!Context.OriginalParentMaterial || Context.SourceInstances.Num() < 2)
		return false;

	// 0. Backup effective params of every source instance (incl. inherited ones),
	//    so we can guarantee no parameter drift after the operation.
	auto Backup = BackupMaterialParams(Context.SourceInstances);

	// 1. Read all params from each source instance
	TArray<FMaterialParamSnapshot> Snapshots;
	for (UMaterialInstanceConstant* Src : Context.SourceInstances)
		Snapshots.Add(ReadAllParams(Src));

	// 2. Calculate mode values
	FMaterialParamSnapshot Modes = CalculateParamModes(Snapshots);

	// 3. Create the shared parent MI
	FString SanitizedDir = Context.OutputDirectory;
	if (!SanitizedDir.EndsWith(TEXT("/"))) SanitizedDir += TEXT("/");

	// 以第一个子材质实例命名 + 后缀 "_Parent"（与自动归并保持一致）
	const FString FirstChildName = (Context.SourceInstances.Num() > 0 && Context.SourceInstances[0])
		? Context.SourceInstances[0]->GetName()
		: Context.OriginalParentMaterial->GetName();
	FString AssetName = FString::Printf(TEXT("%s%s"), *FirstChildName, TEXT("_Parent"));

	FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
	IAssetTools& AssetTools = AssetToolsModule.Get();

	FString UniqueName, UniquePkg;
	AssetTools.CreateUniqueAssetName(SanitizedDir + AssetName, TEXT(""), UniquePkg, UniqueName);

	UMaterialInstanceConstantFactoryNew* Factory = NewObject<UMaterialInstanceConstantFactoryNew>();
	Factory->InitialParent = Context.OriginalParentMaterial;

	UMaterialInstanceConstant* SharedMI = Cast<UMaterialInstanceConstant>(
		AssetTools.CreateAsset(UniqueName, SanitizedDir, UMaterialInstanceConstant::StaticClass(), Factory));

	if (!SharedMI) return false;
	OutCreatedSharedParent = SharedMI;

	// 4. Set mode values on the shared parent
	for (const auto& P : Modes.Scalars)
		SharedMI->SetScalarParameterValueEditorOnly(P.Key, P.Value);
	for (const auto& P : Modes.Vectors)
		SharedMI->SetVectorParameterValueEditorOnly(P.Key, P.Value);
	for (const auto& P : Modes.Textures)
		SharedMI->SetTextureParameterValueEditorOnly(P.Key, P.Value.LoadSynchronous());

	// Static switches on shared parent: apply the mode as overrides
	{
		FStaticParameterSet SharedParams;
		for (const auto& P : Modes.StaticSwitches.SwitchValues)
		{
			FStaticSwitchParameter Sw;
			Sw.ParameterInfo.Name = P.Key;
			Sw.Value = P.Value;
			Sw.bOverride = true;
			SharedParams.StaticSwitchParameters.Add(Sw);
		}
		SharedMI->UpdateStaticPermutation(SharedParams);
	}

	SharedMI->PostEditChange();
	SharedMI->MarkPackageDirty();
	FAssetRegistryModule::AssetCreated(SharedMI);

	// 5. For each child, reparent and set only differing params
	for (int32 i = 0; i < Context.SourceInstances.Num(); ++i)
	{
		UMaterialInstanceConstant* Child = Context.SourceInstances[i];
		if (!Child) continue;

		// Reparent
		Child->SetParentEditorOnly(SharedMI);

		// Clear all existing overrides
		Child->ScalarParameterValues.Empty();
		Child->VectorParameterValues.Empty();
		Child->TextureParameterValues.Empty();

		// Set only params that differ from mode
		const FMaterialParamSnapshot& ChildSnap = Snapshots[i];
		for (const auto& P : ChildSnap.Scalars)
		{
			const float* ModeVal = Modes.Scalars.Find(P.Key);
			if (!ModeVal || !FMath::IsNearlyEqual(*ModeVal, P.Value))
				Child->SetScalarParameterValueEditorOnly(P.Key, P.Value);
		}
		for (const auto& P : ChildSnap.Vectors)
		{
			const FLinearColor* ModeVal = Modes.Vectors.Find(P.Key);
			if (!ModeVal || !ModeVal->Equals(P.Value))
				Child->SetVectorParameterValueEditorOnly(P.Key, P.Value);
		}
		for (const auto& P : ChildSnap.Textures)
		{
			const TSoftObjectPtr<UTexture>* ModeVal = Modes.Textures.Find(P.Key);
			if (!ModeVal || ModeVal->GetUniqueID() != P.Value.GetUniqueID())
				Child->SetTextureParameterValueEditorOnly(P.Key, P.Value.LoadSynchronous());
		}
		Child->PostEditChange();
		Child->MarkPackageDirty();
	}

	// 6. Guarantee source instances' effective parameters are exactly as before.
	RestoreMaterialParams(Backup);

	// 6. Save
	TArray<UPackage*> ToSave;
	ToSave.Add(SharedMI->GetOutermost());
	for (UMaterialInstanceConstant* Src : Context.SourceInstances)
		ToSave.Add(Src->GetOutermost());
	FEditorFileUtils::PromptForCheckoutAndSave(ToSave, false, false);

	return true;
}

// ============================================================================
// Group By Static Switches
// ============================================================================

TMap<FStaticSwitchSignature, TArray<UMaterialInstanceConstant*>>
GroupByStaticSwitches(const TArray<UMaterialInstanceConstant*>& Instances)
{
	TMap<FStaticSwitchSignature, TArray<UMaterialInstanceConstant*>> Groups;
	for (UMaterialInstanceConstant* MI : Instances)
	{
		if (!MI) continue;
		Groups.FindOrAdd(ReadStaticSwitches(MI)).Add(MI);
	}
	return Groups;
}

// ============================================================================
// Generate Switch Suffix
// ============================================================================

FString GenerateSwitchSuffix(const FStaticSwitchSignature& Sig)
{
	if (Sig.SwitchValues.Num() == 0) return TEXT("Default");
	TArray<FName> Keys;
	Sig.SwitchValues.GetKeys(Keys);
	Keys.Sort([](const FName& A, const FName& B) { return A.ToString() < B.ToString(); });
	TArray<FString> Parts;
	for (const FName& K : Keys)
	{
		const bool* V = Sig.SwitchValues.Find(K);
		FString S = K.ToString();
		if (S.Len() > 3) S = S.Left(3);
		Parts.Add(FString::Printf(TEXT("%s%d"), *S, V && *V ? 1 : 0));
	}
	return FString::Join(Parts, TEXT("_"));
}

// ============================================================================
// Create Or Get Shared MI
// ============================================================================

UMaterialInstanceConstant* CreateOrGetSharedMI(
	UMaterialInterface* ParentMat, const FStaticSwitchSignature& Sig,
	const FString& OutputDir, const FString& FirstChildName,
	TSet<UPackage*>& OutModifiedPackages)
{
	if (!ParentMat) return nullptr;

	FString SanitizedDir = OutputDir;
	if (!SanitizedDir.EndsWith(TEXT("/"))) SanitizedDir += TEXT("/");

	// Name the shared MI after its first child instance + suffix "_Parent".
	const FString Name = FirstChildName.IsEmpty()
		? FString::Printf(TEXT("%s_%s"), *ParentMat->GetName(), TEXT("_Parent"))
		: FString::Printf(TEXT("%s%s"), *FirstChildName, TEXT("_Parent"));
	const FString FullPath = SanitizedDir + Name;

	if (UMaterialInstanceConstant* Existing = LoadObject<UMaterialInstanceConstant>(nullptr, *FullPath, nullptr, LOAD_NoWarn))
	{
		if (Existing->Parent == ParentMat && ReadStaticSwitches(Existing) == Sig)
			return Existing;
	}

	FAssetToolsModule& ATM = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
	FString UN, UP;
	ATM.Get().CreateUniqueAssetName(SanitizedDir + Name, TEXT(""), UP, UN);

	UMaterialInstanceConstantFactoryNew* F = NewObject<UMaterialInstanceConstantFactoryNew>();
	F->InitialParent = ParentMat;

	UMaterialInstanceConstant* NewMI = Cast<UMaterialInstanceConstant>(
		ATM.Get().CreateAsset(UN, SanitizedDir, UMaterialInstanceConstant::StaticClass(), F));
	if (!NewMI) return nullptr;

	FStaticParameterSet Params;
	for (const auto& P : Sig.SwitchValues)
	{
		FStaticSwitchParameter Sw;
		Sw.ParameterInfo.Name = P.Key; Sw.Value = P.Value; Sw.bOverride = true;
		Params.StaticSwitchParameters.Add(Sw);
	}
	NewMI->UpdateStaticPermutation(Params);
	NewMI->PostEditChange();
	NewMI->MarkPackageDirty();
	FAssetRegistryModule::AssetCreated(NewMI);
	OutModifiedPackages.Add(NewMI->GetOutermost());
	return NewMI;
}

// ============================================================================
// Consolidate References
// ============================================================================

int32 ConsolidateReferences(UMaterialInstanceConstant* Target, const TArray<UMaterialInstanceConstant*>& Sources)
{
	if (!Target || Sources.Num() == 0) return 0;
	TArray<UObject*> Objs;
	for (auto* S : Sources) if (S && S != Target) Objs.Add(S);
	if (Objs.Num() == 0) return 0;
	ObjectTools::ConsolidateObjects(Target, Objs, false);
	for (auto* O : Objs) if (O != Target) O->MarkAsGarbage();
	return Objs.Num();
}

// ============================================================================
// Reparent Instances (non-destructive)
// ============================================================================

int32 ReparentInstances(UMaterialInstanceConstant* NewParent, const TArray<UMaterialInstanceConstant*>& Instances,
	TSet<UPackage*>& OutModifiedPackages)
{
	if (!NewParent || Instances.Num() == 0) return 0;

	int32 Count = 0;
	const FString ParentPath = NewParent->GetPathName();

	for (UMaterialInstanceConstant* Child : Instances)
	{
		if (!Child || Child == NewParent) continue;

		// Already parented to the shared MI -> nothing to do (not a change).
		if (Child->Parent && Child->Parent->GetPathName() == ParentPath)
		{
			continue;
		}

		// Reparent only. All overridden parameters (scalars / vectors / textures /
		// static switches) on the child are left completely untouched, so its
		// rendered result and parameter values stay identical.
		Child->SetParentEditorOnly(NewParent);
		Child->PostEditChange();
		Child->MarkPackageDirty();
		OutModifiedPackages.Add(Child->GetOutermost());
		Count++;
	}

	return Count;
}

// ============================================================================
// Backup & Restore Material Params (safety net for mutating operations)
// ============================================================================

TMap<UMaterialInstanceConstant*, FMaterialParamSnapshot> BackupMaterialParams(
	const TArray<UMaterialInstanceConstant*>& Materials)
{
	TMap<UMaterialInstanceConstant*, FMaterialParamSnapshot> Backup;
	for (UMaterialInstanceConstant* M : Materials)
		if (M) Backup.Add(M, ReadAllEffectiveParams(M));
	return Backup;
}

int32 RestoreMaterialParams(
	const TMap<UMaterialInstanceConstant*, FMaterialParamSnapshot>& Backup,
	TSet<UPackage*>* OutModifiedPackages)
{
	int32 Restored = 0;
	for (const auto& Pair : Backup)
	{
		UMaterialInstanceConstant* M = Pair.Key;
		if (!M) continue;

		const FMaterialParamSnapshot& Old = Pair.Value;
		const FMaterialParamSnapshot New = ReadAllEffectiveParams(M);
		bool bModified = false;

		// Scalars
		for (const auto& P : Old.Scalars)
		{
			const float* NV = New.Scalars.Find(P.Key);
			if (!NV || !FMath::IsNearlyEqual(*NV, P.Value))
			{
				UMaterialEditingLibrary::SetMaterialInstanceScalarParameterValue(M, P.Key, P.Value);
				bModified = true;
			}
		}
		// Vectors
		for (const auto& P : Old.Vectors)
		{
			const FLinearColor* NV = New.Vectors.Find(P.Key);
			if (!NV || !NV->Equals(P.Value))
			{
				UMaterialEditingLibrary::SetMaterialInstanceVectorParameterValue(M, P.Key, P.Value);
				bModified = true;
			}
		}
		// Textures
		for (const auto& P : Old.Textures)
		{
			UTexture* OldTex = P.Value.LoadSynchronous();
			UTexture* NewTex = New.Textures.Contains(P.Key) ? New.Textures[P.Key].LoadSynchronous() : nullptr;
			if (OldTex != NewTex)
			{
				UMaterialEditingLibrary::SetMaterialInstanceTextureParameterValue(M, P.Key, OldTex);
				bModified = true;
			}
		}
		// Static Switches (booleans)
		for (const auto& P : Old.StaticSwitches.SwitchValues)
		{
			const bool* NV = New.StaticSwitches.SwitchValues.Find(P.Key);
			if (!NV || *NV != P.Value)
			{
				UMaterialEditingLibrary::SetMaterialInstanceStaticSwitchParameterValue(M, P.Key, P.Value);
				bModified = true;
			}
		}

		if (bModified)
		{
			M->PostEditChange();
			M->MarkPackageDirty();
			if (OutModifiedPackages) OutModifiedPackages->Add(M->GetOutermost());
			Restored++;
		}
	}
	return Restored;
}

#undef LOCTEXT_NAMESPACE
