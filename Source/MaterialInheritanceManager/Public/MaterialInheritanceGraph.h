// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraph.h"
#include "AssetRegistry/AssetData.h"
#include "MaterialInheritanceIndex.h"
#include "MaterialInheritanceGraph.generated.h"

class UMaterialInterface;
class UMaterialInheritanceGraphNode;

UCLASS()
class UMaterialInheritanceGraph : public UEdGraph
{
	GENERATED_BODY()

public:
	void ClearGraph();
	void RebuildGraph(UMaterialInterface* SelectedMaterial, int32 InMaxDepth, int32 InMaxNodes, bool bProgressive = false);
	void OnExpandCollapsed(UMaterialInheritanceGraphNode* PlaceholderNode);

private:
	/** Result of laying out one subtree: its vertical center and the created node. */
	struct FSubtreeResult { float CenterY = 0.f; UMaterialInheritanceGraphNode* Node = nullptr; };

	TArray<FAssetData> GetChildren(const FAssetData& Asset);
	FAssetData GetParentAssetData(const FAssetData& Asset) const;

	/** Recursively lays out the descendants of Asset on the LEFT side.
	 *  All nodes claimed by this subtree advance the shared CursorY so sibling
	 *  subtrees never overlap. Returns the placed node + its vertical center.
	 *  bNoDepthLimit / bNoCountLimit are inherited from an expanded ancestor and
	 *  additionally OR'ed with this node's own expansion sets, so double-clicking
	 *  a "..." node reveals ONLY that node's subtree, never the whole graph. */
	FSubtreeResult LayoutSubtree(const FAssetData& Asset, int32 Depth, float& CursorY,
		int32& RemainingBudget, bool bNoDepthLimit, bool bNoCountLimit, int32& TotalPlaced);

	UMaterialInheritanceGraphNode* PlaceNode(const FAssetData& Asset, int32 Depth, float X, float Y);
	UMaterialInheritanceGraphNode* PlaceCollapsedNode(const FAssetData& Parent, int32 Depth, float X, float Y, int32 HiddenCount, bool bDepthCollapse = false);

	float ColW = 380.0f, RowH = 90.0f, CenterX = 0.0f;
	int32 MaxDepth = 4, MaxNodeCount = 200;

	/** Nodes whose subtree should ignore the depth limit (expanded depth "..."). */
	TSet<FSoftObjectPath> ExpandedDepth;
	/** Nodes whose subtree should ignore the node-count limit (expanded count "..."). */
	TSet<FSoftObjectPath> ExpandedCount;

	/** Root material the graph is centered on (used when expanding a "..." node). */
	FSoftObjectPath RootAssetPath;
	FMaterialInheritanceIndex InheritanceIndex;
};
