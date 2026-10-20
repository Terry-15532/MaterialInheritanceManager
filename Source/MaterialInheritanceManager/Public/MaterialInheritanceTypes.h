// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

class UMaterialInterface;
class UMaterialInstanceConstant;
class UMaterial;
class UTexture;

// ============================================================================
// Static Switch Signature (unchanged)
// ============================================================================
struct FStaticSwitchSignature
{
	TMap<FName, bool> SwitchValues;
	bool IsEmpty() const { return SwitchValues.Num() == 0; }
	bool operator==(const FStaticSwitchSignature& Other) const;
	FString ToDisplayString() const;
	friend uint32 GetTypeHash(const FStaticSwitchSignature& Sig)
	{
		uint32 Hash = 0;
		for (const auto& Pair : Sig.SwitchValues)
			Hash = HashCombine(Hash, GetTypeHash(Pair.Key)) ^ HashCombine(Hash, GetTypeHash(Pair.Value));
		return Hash;
	}
};

// ============================================================================
// Full Parameter Snapshot (scalars + vectors + textures + static switches)
// ============================================================================
struct FMaterialParamSnapshot
{
	TMap<FName, float> Scalars;
	TMap<FName, FLinearColor> Vectors;
	TMap<FName, TSoftObjectPtr<UTexture>> Textures;
	FStaticSwitchSignature StaticSwitches;
	// Verification only: local checkbox states, keyed by type + full parameter identity.
	// Kept separate from effective values used by merge/restore operations.
	TMap<FString, bool> OverrideStates;

	bool IsEmpty() const;
	FString ToDisplayString() const;
};

// ============================================================================
// Merge Group Info
// ============================================================================
struct FMergeGroupInfo
{
	FStaticSwitchSignature SwitchSignature;
	TArray<struct FAssetData> Members;
	FString SuggestedName;
	FString TargetMIPath;
	bool bWillCreateNew = true;
	int32 MemberCount = 0;
};

// ============================================================================
// Shared Parent creation context
// ============================================================================
struct FSharedParentContext
{
	// The parent the new shared MI should sit under. This is the DIRECT parent
	// of the source instances (i.e. the "原本的材质的母材质"), not the base
	// UMaterial several levels up the inheritance chain.
	UMaterialInterface* OriginalParentMaterial = nullptr;
	TArray<UMaterialInstanceConstant*> SourceInstances;
	FString OutputDirectory;
};

// ============================================================================
// Functions
// ============================================================================

/** Read all overridden parameters from a MaterialInstance. */
FMaterialParamSnapshot ReadAllParams(UMaterialInstanceConstant* MI);

/** Read all *effective* (inheritance-resolved) parameters from a MaterialInterface.
 *  Use this for snapshot comparison so that reverting a parameter to its inherited
 *  value is not mistaken for a change. */
FMaterialParamSnapshot ReadAllEffectiveParams(UMaterialInterface* Mat);

/** Effective values plus this instance's local override checkbox states. */
FMaterialParamSnapshot ReadMaterialVerificationSnapshot(UMaterialInterface* Mat);
/** Append checkbox differences; returns true even if all effective values are unchanged. */
bool CompareMaterialOverrideStates(const FMaterialParamSnapshot& Before,
	const FMaterialParamSnapshot& After, FString& OutDiff);

/** Backup the *effective* (inheritance-resolved) parameters of the given material instances.
 *  Use before a mutating operation: even values inherited from a parent are captured, because
 *  the parent may change during the operation. */
TMap<UMaterialInstanceConstant*, FMaterialParamSnapshot> BackupMaterialParams(
	const TArray<UMaterialInstanceConstant*>& Materials);

/** After a mutating operation, force every backed-up material's effective parameters back to
 *  the backed-up values by re-applying overrides where needed. Returns the number of materials
 *  that required a change. This guarantees operations never leave a material's parameters altered.
 *  If supplied, OutModifiedPackages also collects packages changed by this safety net. */
int32 RestoreMaterialParams(
	const TMap<UMaterialInstanceConstant*, FMaterialParamSnapshot>& Backup,
	TSet<UPackage*>* OutModifiedPackages = nullptr);

/** Read static-switch values only. */
FStaticSwitchSignature ReadStaticSwitches(UMaterialInstanceConstant* MI);

/** Calculate the mode (most frequent value) for each parameter across all instances. */
FMaterialParamSnapshot CalculateParamModes(const TArray<FMaterialParamSnapshot>& Snapshots);

/**
 * Create a shared parent MI that inherits from the same parent as the source instances.
 * Sets mode values as defaults, then for each child: only overrides parameters that differ from the mode.
 * Returns true on success, and outputs the newly created shared parent MI.
 */
bool CreateSharedParentAndReparent(const FSharedParentContext& Context, UMaterialInstanceConstant*& OutCreatedSharedParent);

/** Group MICs by identical static-switch combination. */
TMap<FStaticSwitchSignature, TArray<UMaterialInstanceConstant*>>
GroupByStaticSwitches(const TArray<UMaterialInstanceConstant*>& Instances);

/** Generate a name suffix from a switch signature. */
FString GenerateSwitchSuffix(const FStaticSwitchSignature& Signature);

/** Create or get a shared MI with specific static-switch overrides.
 *  The created MI is named as the first child instance's name + "_Parent".
 *  ParentMaterial may be a base UMaterial or an intermediate UMaterialInstanceConstant.
 *  Adds new packages to OutModifiedPackages without saving; the caller saves the whole operation. */
UMaterialInstanceConstant* CreateOrGetSharedMI(
	UMaterialInterface* ParentMaterial,
	const FStaticSwitchSignature& Signature,
	const FString& OutputDir,
	const FString& FirstChildName,
	TSet<UPackage*>& OutModifiedPackages);

/** Consolidate all references from Sources to Target. */
int32 ConsolidateReferences(
	UMaterialInstanceConstant* Target,
	const TArray<UMaterialInstanceConstant*>& Sources);

/**
 * Non-destructively reparent instances to NewParent, preserving every parameter
 * override (scalars, vectors, textures, static switches) on the instances.
 * No asset is deleted. Returns the number of instances reparented.
 * Adds changed packages to OutModifiedPackages without saving.
 */
int32 ReparentInstances(
	UMaterialInstanceConstant* NewParent,
	const TArray<UMaterialInstanceConstant*>& Instances,
	TSet<UPackage*>& OutModifiedPackages);
