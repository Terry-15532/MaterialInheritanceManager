// Copyright Epic Games, Inc. All Rights Reserved.

#include "MaterialInheritanceGraphNode.h"
#include "Materials/Material.h"
#include "Materials/MaterialInstanceConstant.h"
#include "EdGraph/EdGraphPin.h"
#include "MaterialInheritanceTypes.h"

#define LOCTEXT_NAMESPACE "MaterialInheritanceGraphNode"

UMaterialInterface* UMaterialInheritanceGraphNode::GetMaterialAsset()
{
	if (MaterialAsset) return MaterialAsset;

	if (AssetPath.IsValid())
	{
		MaterialAsset = Cast<UMaterialInterface>(AssetPath.TryLoad());
		if (MaterialAsset)
		{
			if (auto* Mi = Cast<UMaterialInstanceConstant>(MaterialAsset))
				SwitchSummary = ReadStaticSwitches(Mi).ToDisplayString();
		}
	}
	return MaterialAsset;
}

void UMaterialInheritanceGraphNode::AllocateDefaultPins()
{
	FEdGraphPinType PT; PT.PinCategory = TEXT("Material");
	CreatePin(EGPD_Output, PT, TEXT("父"))->bHidden = false;
	CreatePin(EGPD_Input,  PT, TEXT("子"))->bHidden = false;
}

FText UMaterialInheritanceGraphNode::GetPinDisplayName(const UEdGraphPin* Pin) const
{
	return Pin->PinName.ToString().Equals(TEXT("父")) ? LOCTEXT("POut","父") : LOCTEXT("PIn","子");
}

FText UMaterialInheritanceGraphNode::GetNodeTitle(ENodeTitleType::Type) const
{
	if (bIsCollapsedPlaceholder) return LOCTEXT("Collapsed","...");
	FString N = CachedName;
	if (N.IsEmpty() && MaterialAsset) N = MaterialAsset->GetName();
	if (N.IsEmpty()) return LOCTEXT("Unknown","(未知)");
	if (N.Len() > 28) N = N.Left(25) + TEXT("...");
	return FText::FromString(FString::Printf(TEXT("%s (%s)"), *N, bIsBaseMaterial ? TEXT("Material") : TEXT("MI")));
}

FText UMaterialInheritanceGraphNode::GetTooltipText() const
{
	if (bIsCollapsedPlaceholder)
	{
		return FText::Format(
			bIsDepthCollapse ? LOCTEXT("TipDepth","双击展开更多层级（隐藏 {0} 个子项）")
			                 : LOCTEXT("TipCount","双击展开更多节点（隐藏 {0} 个子项）"),
			FText::AsNumber(CollapsedHiddenCount));
	}

	UMaterialInterface* MA = const_cast<UMaterialInheritanceGraphNode*>(this)->GetMaterialAsset();
	FTextBuilder B;
	if (MA)
	{
		B.AppendLine(FText::FromString(MA->GetPathName()));
		B.AppendLine(FText::Format(LOCTEXT("TF","类型:{0}"), FText::FromString(MA->GetClass()->GetName())));
		if (!SwitchSummary.IsEmpty() && SwitchSummary != TEXT("(none)"))
			B.AppendLine(FText::Format(LOCTEXT("SF","开关:{0}"), FText::FromString(SwitchSummary)));
	}
	else
	{
		B.AppendLine(FText::FromString(AssetPath.ToString()));
		B.AppendLine(LOCTEXT("Unloaded","（资产尚未加载，悬停时才加载）"));
	}
	return B.ToText();
}

FLinearColor UMaterialInheritanceGraphNode::GetNodeTitleColor() const
{
	if (bIsCurrent) return FLinearColor(1.0f,0.60f,0.0f);
	if (bIsCollapsedPlaceholder) return bIsDepthCollapse ? FLinearColor(0.45f,0.30f,0.05f) : FLinearColor(0.10f,0.10f,0.10f);
	if (Depth==0) return FLinearColor(0.15f,0.30f,0.55f);
	if (Depth<0)  return FLinearColor(0.05f,0.12f,0.35f);
	return FLinearColor(0.06f,0.22f,0.10f);
}

FLinearColor UMaterialInheritanceGraphNode::GetNodeBodyTintColor() const
{
	if (bIsCurrent) return FLinearColor(0.32f,0.19f,0.02f);
	if (bIsCollapsedPlaceholder) return bIsDepthCollapse ? FLinearColor(0.20f,0.13f,0.02f) : FLinearColor(0.05f,0.05f,0.05f);
	if (Depth==0) return FLinearColor(0.08f,0.16f,0.30f);
	if (Depth<0)  return FLinearColor(0.03f,0.07f,0.18f);
	return FLinearColor(0.03f,0.11f,0.05f);
}

#undef LOCTEXT_NAMESPACE
