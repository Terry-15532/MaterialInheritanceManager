#include "MaterialInheritanceIndex.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetRegistry/IAssetRegistry.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Misc/PackageName.h"
#include "UObject/UObjectIterator.h"

void FMaterialInheritanceIndex::Build()
{
	Assets.Reset();
	Parents.Reset();
	Children.Reset();
	IAssetRegistry& Registry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry")).Get();
	TArray<FAssetData> Instances;
	Registry.GetAssetsByClass(UMaterialInstanceConstant::StaticClass()->GetClassPathName(), Instances, true);
	for (const FAssetData& Data : Instances)
	{
		const FSoftObjectPath Path = Data.GetSoftObjectPath();
		Assets.Add(Path, Data);
		FString ParentTag;
		if (Data.GetTagValue(FName(TEXT("Parent")), ParentTag) && !ParentTag.IsEmpty())
		{
			const FSoftObjectPath ParentPath(FPackageName::ExportTextPathToObjectPath(ParentTag));
			if (ParentPath.IsValid()) Parents.Add(Path, Registry.GetRedirectedObjectPath(ParentPath));
		}
	}

	// A rename/reparent updates the UObject immediately, but child packages may still
	// contain the old Parent tag until saved. Never load every asset just to repair that.
	for (TObjectIterator<UMaterialInstanceConstant> It; It; ++It)
	{
		UMaterialInstanceConstant* Instance = *It;
		if (!IsValid(Instance) || Instance->IsTemplate() || Instance->HasAnyFlags(RF_Transient)) continue;
		const FSoftObjectPath Path(Instance);
		Assets.Add(Path, FAssetData(Instance));
		if (Instance->Parent)
		{
			Parents.Add(Path, FSoftObjectPath(Instance->Parent));
			Assets.Add(FSoftObjectPath(Instance->Parent), FAssetData(Instance->Parent));
		}
		else Parents.Remove(Path);
	}
	for (const auto& Pair : Parents)
	{
		if (const FAssetData* Child = Assets.Find(Pair.Key))
			Children.FindOrAdd(Pair.Value).Add(*Child);
	}
	for (auto& Pair : Children)
		Pair.Value.Sort([](const FAssetData& A, const FAssetData& B) { return A.AssetName.LexicalLess(B.AssetName); });
}

TArray<FAssetData> FMaterialInheritanceIndex::GetChildren(const FSoftObjectPath& Parent) const
{
	if (const TArray<FAssetData>* Found = Children.Find(Parent)) return *Found;
	return {};
}

FAssetData FMaterialInheritanceIndex::GetParent(const FSoftObjectPath& Child) const
{
	if (const FSoftObjectPath* Parent = Parents.Find(Child))
	{
		if (const FAssetData* Found = Assets.Find(*Parent)) return *Found;
		IAssetRegistry& Registry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry")).Get();
		return Registry.GetAssetByObjectPath(*Parent);
	}
	return {};
}
