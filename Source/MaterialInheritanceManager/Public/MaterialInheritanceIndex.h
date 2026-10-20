#pragma once

#include "CoreMinimal.h"
#include "AssetRegistry/AssetData.h"

/** One registry scan per operation, with live objects taking precedence over stale disk tags. */
struct FMaterialInheritanceIndex
{
	void Build();
	TArray<FAssetData> GetChildren(const FSoftObjectPath& Parent) const;
	FAssetData GetParent(const FSoftObjectPath& Child) const;

private:
	TMap<FSoftObjectPath, FAssetData> Assets;
	TMap<FSoftObjectPath, FSoftObjectPath> Parents;
	TMap<FSoftObjectPath, TArray<FAssetData>> Children;
};
