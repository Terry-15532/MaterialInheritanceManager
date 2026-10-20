// Copyright Epic Games, Inc. All Rights Reserved.

#include "MaterialInheritanceGraph.h"
#include "MaterialInheritanceGraphNode.h"
#include "MaterialInheritanceGraphSchema.h"
#include "MaterialInheritanceTypes.h"

#include "Materials/Material.h"
#include "Materials/MaterialInstanceConstant.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetRegistry/IAssetRegistry.h"
#include "EdGraph/EdGraphPin.h"

void UMaterialInheritanceGraph::ClearGraph() { TArray<UEdGraphNode*> All = Nodes; for (UEdGraphNode* N : All) RemoveNode(N); }

FAssetData UMaterialInheritanceGraph::GetParentAssetData(const FAssetData& A) const
{
	return InheritanceIndex.GetParent(A.GetSoftObjectPath());
}

TArray<FAssetData> UMaterialInheritanceGraph::GetChildren(const FAssetData& A)
{
	return InheritanceIndex.GetChildren(A.GetSoftObjectPath());
}

UMaterialInheritanceGraphNode* UMaterialInheritanceGraph::PlaceNode(const FAssetData& A, int32 D, float X, float Y)
{
	// 注意：这里只记录路径与名称，不加载真实资产。一次性加载成百上千个
	// 材质实例是之前卡顿的根源。资产在用户点击或悬浮时由 GetMaterialAsset() 按需加载。
	FGraphNodeCreator<UMaterialInheritanceGraphNode> C(*this); UMaterialInheritanceGraphNode* N = C.CreateNode();
	N->AssetPath = A.GetSoftObjectPath();
	N->CachedName = A.AssetName.ToString();
	N->Depth = D;
	N->bIsBaseMaterial = A.GetClass() && A.GetClass()->IsChildOf(UMaterial::StaticClass());
	N->NodePosX = X; N->NodePosY = Y;
	C.Finalize(); return N;
}

UMaterialInheritanceGraphNode* UMaterialInheritanceGraph::PlaceCollapsedNode(const FAssetData& P, int32 D, float X, float Y, int32 HC, bool bDepthCollapse)
{
	FGraphNodeCreator<UMaterialInheritanceGraphNode> C(*this); UMaterialInheritanceGraphNode* N = C.CreateNode();
	N->bIsCollapsedPlaceholder = true; N->bIsDepthCollapse = bDepthCollapse; N->CollapsedParentPath = P.GetSoftObjectPath(); N->CollapsedHiddenCount = HC; N->Depth = D; N->NodePosX = X; N->NodePosY = Y;
	C.Finalize(); return N;
}

/** Recursively lay out the descendants of Asset on the LEFT side.
 *  Children are placed one column further left (column = -Depth). Each subtree
 *  advances the shared CursorY, so no two subtrees overlap vertically.
 *  Connection rule: child.父(Output, right pin) -> parent.子(Input, left pin),
 *  which produces short, non-crossing lines given parent-right / child-left. */
namespace
{
	// Hard safety cap so an over-eager expansion can never freeze the editor.
	static const int32 HardMaxNodes = 3000;
}

/** Recursively lay out the descendants of Asset on the LEFT side.
 *  Children are placed one column further left (column = -Depth). Each subtree
 *  advances the shared CursorY, so no two subtrees overlap vertically.
 *  Connection rule: child.父(Output, right pin) -> parent.子(Input, left pin),
 *  which produces short, non-crossing lines given parent-right / child-left.
 *
 *  bNoDepthLimit / bNoCountLimit are inherited from an expanded ancestor; they
 *  are OR'ed with this node's own ExpandedDepth / ExpandedCount membership, so
 *  double-clicking a "..." node lifts the limit for ONLY that node's subtree. */
UMaterialInheritanceGraph::FSubtreeResult UMaterialInheritanceGraph::LayoutSubtree(const FAssetData& A, int32 Depth, float& CursorY, int32& RemainingBudget, bool bNoDepthLimit, bool bNoCountLimit, int32& TotalPlaced)
{
	const float X = -Depth * ColW;
	TArray<FAssetData> Ch = GetChildren(A);
	const bool bHasKids = Ch.Num() > 0;

	const bool bThisNoDepth = bNoDepthLimit || ExpandedDepth.Contains(A.GetSoftObjectPath());
	const bool bThisNoCount = bNoCountLimit || ExpandedCount.Contains(A.GetSoftObjectPath());

	// Count limit reached (or hard safety cap) but still has children -> count "...".
	if (bHasKids && !bThisNoCount && (RemainingBudget <= 0 || TotalPlaced >= HardMaxNodes))
	{
		UMaterialInheritanceGraphNode* N = PlaceCollapsedNode(A, Depth, X, CursorY, Ch.Num(), /*bDepthCollapse=*/false);
		CursorY += RowH; TotalPlaced++; if (!bThisNoCount) RemainingBudget--;
		return { CursorY - RowH, N };
	}
	// Depth limit reached but still has children -> depth "...".
	if (bHasKids && !bThisNoDepth && Depth >= MaxDepth)
	{
		UMaterialInheritanceGraphNode* N = PlaceCollapsedNode(A, Depth, X, CursorY, Ch.Num(), /*bDepthCollapse=*/true);
		CursorY += RowH; TotalPlaced++;
		return { CursorY - RowH, N };
	}
	// True leaf: no children -> draw as a normal node.
	if (!bHasKids)
	{
		UMaterialInheritanceGraphNode* N = PlaceNode(A, Depth, X, CursorY);
		CursorY += RowH; TotalPlaced++; if (!bThisNoCount) RemainingBudget--;
		return { CursorY - RowH, N };
	}

	// Internal node: lay out all children first, then center this node on them.
	TArray<UMaterialInheritanceGraphNode*> ChildNodes;
	TArray<float> ChildCenters;
	for (int32 i = 0; i < Ch.Num(); ++i)
	{
		if (!bThisNoCount && (RemainingBudget <= 0 || TotalPlaced >= HardMaxNodes))
		{
			UMaterialInheritanceGraphNode* Col = PlaceCollapsedNode(A, Depth + 1, X - ColW, CursorY, Ch.Num() - i, /*bDepthCollapse=*/false);
			CursorY += RowH; TotalPlaced++; if (!bThisNoCount) RemainingBudget--;
			ChildNodes.Add(Col); ChildCenters.Add(CursorY - RowH);
			break;
		}
		FSubtreeResult K = LayoutSubtree(Ch[i], Depth + 1, CursorY, RemainingBudget, bThisNoDepth, bThisNoCount, TotalPlaced);
		ChildNodes.Add(K.Node); ChildCenters.Add(K.CenterY);
	}

	const float CenterY = ChildCenters.Num() ? (ChildCenters[0] + ChildCenters.Last()) * 0.5f : CursorY;
	UMaterialInheritanceGraphNode* N = PlaceNode(A, Depth, X, CenterY);
	TotalPlaced++; if (!bThisNoCount) RemainingBudget--;

	// Connect each child's 父 (Output) pin to this node's 子 (Input) pin.
	for (UMaterialInheritanceGraphNode* C : ChildNodes)
	{
		UEdGraphPin* COut = C->FindPin(TEXT("父"), EGPD_Output);
		UEdGraphPin* NIn  = N->FindPin(TEXT("子"), EGPD_Input);
		if (COut && NIn) COut->MakeLinkTo(NIn);
	}
	return { CenterY, N };
}

void UMaterialInheritanceGraph::OnExpandCollapsed(UMaterialInheritanceGraphNode* PN)
{
	if (!PN || !PN->bIsCollapsedPlaceholder) return;

	// Mark ONLY this node's subtree to ignore the relevant limit. No global
	// MaxDepth / MaxNodeCount change, so other "..." nodes stay collapsed.
	if (PN->bIsDepthCollapse)
		ExpandedDepth.Add(PN->CollapsedParentPath);
	else
		ExpandedCount.Add(PN->CollapsedParentPath);

	if (!RootAssetPath.IsValid()) return;

	FAssetRegistryModule& M = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	FAssetData AD = M.Get().GetAssetByObjectPath(RootAssetPath);
	if (AD.IsValid())
		if (UMaterialInterface* MI = Cast<UMaterialInterface>(AD.GetAsset()))
			RebuildGraph(MI, MaxDepth, MaxNodeCount, true);
}

void UMaterialInheritanceGraph::RebuildGraph(UMaterialInterface* Sel, int32 InMaxDepth, int32 InMaxNodes, bool bProgressive)
{
	ClearGraph();
	if (!Sel) { NotifyGraphChanged(); return; }
	InheritanceIndex.Build();

	FSoftObjectPath NewRoot(Sel);
	if (NewRoot != RootAssetPath)
	{
		// Selecting a different material resets any per-node expansions.
		ExpandedDepth.Empty();
		ExpandedCount.Empty();
	}
	MaxDepth = InMaxDepth; MaxNodeCount = InMaxNodes;
	RootAssetPath = NewRoot;
	(void)bProgressive;

	FAssetRegistryModule& M = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	IAssetRegistry& AR = M.Get();
	FAssetData SA = AR.GetAssetByObjectPath(FSoftObjectPath(Sel));
	if (!SA.IsValid()) SA = FAssetData(Sel);

	int32 Total = 0;
	float CursorY = 0.f;

	// ---------------- Children side (left of center) ----------------
	TArray<FAssetData> DC = GetChildren(SA);
	TArray<UMaterialInheritanceGraphNode*> ChildNodes;
	TArray<float> ChildCenters;
	int32 Remaining = MaxNodeCount;   // shared budget across the top-level subtrees
	// 如果之前双击展开过这个因数量上限收起的节点，就不再收起它，而是把全部子节点都画出来。
	const bool bRootNoCount = ExpandedCount.Contains(SA.GetSoftObjectPath());
	for (int32 i = 0; i < DC.Num(); ++i)
	{
		if (!bRootNoCount && (Remaining <= 0 || Total >= HardMaxNodes))
		{
			UMaterialInheritanceGraphNode* Col = PlaceCollapsedNode(SA, 1, -ColW, CursorY, DC.Num() - i, /*bDepthCollapse=*/false);
			CursorY += RowH; Total++; Remaining--;
			ChildNodes.Add(Col); ChildCenters.Add(CursorY - RowH);
			break;
		}
		FSubtreeResult K = LayoutSubtree(DC[i], 1, CursorY, Remaining, false, false, Total);
		ChildNodes.Add(K.Node); ChildCenters.Add(K.CenterY);
	}

	// Selected node centered vertically on its children.
	const float SAY = ChildCenters.Num() ? (ChildCenters[0] + ChildCenters.Last()) * 0.5f : 0.f;
	UMaterialInheritanceGraphNode* CN = PlaceNode(SA, 0, CenterX, SAY);
	CN->bIsCurrent = true;
	Total++;
	for (UMaterialInheritanceGraphNode* C : ChildNodes)
	{
		UEdGraphPin* COut = C->FindPin(TEXT("父"), EGPD_Output);
		UEdGraphPin* NIn  = CN->FindPin(TEXT("子"), EGPD_Input);
		if (COut && NIn) COut->MakeLinkTo(NIn);
	}

	// ---------------- Parent / ancestor side (right of center) ----------------
	// Walk up the parent chain; each ancestor sits one column to the right,
	// aligned with the selected node. Connect child.父 -> parent.子.
	// Uses its own counter so ancestor display is never starved by the child budget.
	FAssetData Cur = SA;
	UMaterialInheritanceGraphNode* ChildNode = CN;
	int32 Col = 1;
	int32 PNC = 0;
	for (;;)
	{
		FAssetData P = GetParentAssetData(Cur);
		if (!P.IsValid()) break;
		FAssetData PD = P;
		if (PNC >= MaxNodeCount) break;
		if (Col > MaxDepth) break;

		UMaterialInheritanceGraphNode* PN = PlaceNode(PD, -Col, Col * ColW, SAY);
		PNC++;
		UEdGraphPin* COut = ChildNode->FindPin(TEXT("父"), EGPD_Output);
		UEdGraphPin* PIn  = PN->FindPin(TEXT("子"), EGPD_Input);
		if (COut && PIn) COut->MakeLinkTo(PIn);
		ChildNode = PN;
		Cur = PD;
		Col++;
	}

	NotifyGraphChanged();
}
