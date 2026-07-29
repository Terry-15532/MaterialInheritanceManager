// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraphNode.h"
#include "MaterialInheritanceGraphNode.generated.h"

class UMaterialInterface;

UCLASS()
class UMaterialInheritanceGraphNode : public UEdGraphNode
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TObjectPtr<UMaterialInterface> MaterialAsset;

	// 构建时只保存路径与名称，避免一次性从磁盘加载成百上千个资产。
	// 真实资产在用户点击或悬浮时才按需加载（见 GetMaterialAsset）。
	UPROPERTY()
	FSoftObjectPath AssetPath;

	UPROPERTY()
	FString CachedName;

	UPROPERTY()
	FString SwitchSummary;

	UPROPERTY()
	bool bIsBaseMaterial = false;

	/** True when this node is the material currently being viewed. */
	UPROPERTY()
	bool bIsCurrent = false;

	// 按需加载底层材质/材质实例，并缓存结果。返回 null 表示加载失败。
	UFUNCTION(BlueprintCallable, Category = "Material Inheritance")
	UMaterialInterface* GetMaterialAsset();

	/** 0 = selected, negative = ancestor, positive = descendant */
	UPROPERTY()
	int32 Depth = 0;

	/** True if this is the "..." collapsed placeholder node. */
	UPROPERTY()
	bool bIsCollapsedPlaceholder = false;

	/** True when the placeholder stands for hidden DEEPER levels (expand increases depth);
	 *  false when it stands for nodes hidden by the node-count limit (expand increases count). */
	UPROPERTY()
	bool bIsDepthCollapse = false;

	/** The parent asset that this "..." node should expand. */
	UPROPERTY()
	FSoftObjectPath CollapsedParentPath;

	/** Number of child items hidden behind this placeholder (for tooltip display). */
	UPROPERTY()
	int32 CollapsedHiddenCount = 0;

	virtual void AllocateDefaultPins() override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FText GetTooltipText() const override;
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual FLinearColor GetNodeBodyTintColor() const override;
	virtual FText GetPinDisplayName(const UEdGraphPin* Pin) const override;
	virtual bool CanDuplicateNode() const override { return false; }
	virtual bool CanUserDeleteNode() const override { return false; }
};
