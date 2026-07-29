// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

#include "MaterialInheritanceTypes.h"

class UMaterialInheritanceManagerConfig;
class SMaterialInheritancePanel;
class FAssetThumbnailPool;

/**
 * Right-side panel with material preview thumbnail + operation tabs.
 */
class SMaterialInheritanceSidebar : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SMaterialInheritanceSidebar)
		: _OwnerPanel(nullptr)
	{}
		SLATE_ARGUMENT(SMaterialInheritancePanel*, OwnerPanel)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

private:
	TSharedRef<SWidget> BuildPreviewPanel();
	TSharedRef<SWidget> BuildVerificationTab();
	TSharedRef<SWidget> BuildSharedParentTab();
	TSharedRef<SWidget> BuildBatchRedirectTab();
	TSharedRef<SWidget> BuildAutoMergeTab();
	TSharedRef<SWidget> BuildSettingsTab();

	void RefreshPreviewThumbnail();

	// 名称显示：普通文本 -> 点击变为可输入文本框 -> 提交后直接改名
	TSharedPtr<class SBox> NameBox;
	bool bEditingName = false;
	TWeakObjectPtr<UObject> LastNameAsset;
	FString LastNameStr;
	void UpdateNameWidget();
	FReply OnNameClicked();
	void OnNameCommitted(const FText& NewText, ETextCommit::Type CommitType);

	SMaterialInheritancePanel* OwnerPanel = nullptr;
	TSharedPtr<FAssetThumbnailPool> ThumbnailPool;
	TSharedPtr<class STextBlock> PreviewPlaceholder;
	TSharedPtr<class SBox> ThumbnailBox;

	// Verification tab
	TMap<FSoftObjectPath, FMaterialParamSnapshot> SnapshotData;
	TSharedPtr<class SMultiLineEditableTextBox> VerificationLog;
	TSharedPtr<class SEditableTextBox> VerifyDirInput;
};
