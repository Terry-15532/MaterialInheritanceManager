// Copyright Epic Games, Inc. All Rights Reserved.

#include "SMaterialInheritanceSidebar.h"
#include "SMaterialInheritancePanel.h"
#include "MaterialInheritanceManagerConfig.h"
#include "MaterialInheritanceGraphNode.h"
#include "MaterialInheritanceTypes.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Materials/MaterialInstanceConstant.h"

#include "ThumbnailRendering/ThumbnailManager.h"
#include "AssetThumbnail.h"

#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Input/SMultiLineEditableTextBox.h"
#include "Widgets/Input/SSpinBox.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SScrollBox.h"
#include "EditorStyleSet.h"
#include "Misc/MessageDialog.h"
#include "Framework/Application/SlateApplication.h"
#include "Styling/AppStyle.h"

#define LOCTEXT_NAMESPACE "MaterialInheritanceSidebar"

void SMaterialInheritanceSidebar::Construct(const FArguments& InArgs)
{
	OwnerPanel = InArgs._OwnerPanel;
	ThumbnailPool = MakeShareable(new FAssetThumbnailPool(8));

	ChildSlot
	[
		SNew(SScrollBox)
		+ SScrollBox::Slot().Padding(4)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot().AutoHeight() [ BuildPreviewPanel() ]
			+ SVerticalBox::Slot().AutoHeight().Padding(0,4,0,0) [ BuildSharedParentTab() ]
			+ SVerticalBox::Slot().AutoHeight().Padding(0,4,0,0) [ BuildBatchRedirectTab() ]
			+ SVerticalBox::Slot().AutoHeight().Padding(0,4,0,0) [ BuildAutoMergeTab() ]
			+ SVerticalBox::Slot().AutoHeight().Padding(0,4,0,0) [ BuildSettingsTab() ]
			+ SVerticalBox::Slot().AutoHeight().Padding(0,4,0,0) [ BuildVerificationTab() ]
		]
	];

	// Register a tick to keep the preview thumbnail up-to-date
	RegisterActiveTimer(0.f, FWidgetActiveTimerDelegate::CreateLambda([this](double, float) -> EActiveTimerReturnType
	{
		RefreshPreviewThumbnail();
		UpdateNameWidget();
		// Re-register: tick again in 2 seconds (this is event-driven via refresh, not pure timer)
		return EActiveTimerReturnType::Continue;
	}));

	UpdateNameWidget();
}

void SMaterialInheritanceSidebar::RefreshPreviewThumbnail()
{
	if (!OwnerPanel || !ThumbnailBox.IsValid()) return;

	UObject* Asset = OwnerPanel->GetSelectedAsset();

	// Track the last shown asset to avoid unnecessary rebuilds
	static TWeakObjectPtr<UObject> LastAsset;
	if (Asset == LastAsset.Get()) return;
	LastAsset = Asset;

	if (Asset)
	{
		TSharedPtr<FAssetThumbnail> Thumb = MakeShareable(new FAssetThumbnail(
			Asset, 120, 120, ThumbnailPool));
		ThumbnailBox->SetContent(
			SNew(SBox).WidthOverride(120).HeightOverride(120)
			[
				Thumb->MakeThumbnailWidget()
			]);
	}
	else
	{
		ThumbnailBox->SetContent(
			SNew(SBox).HAlign(HAlign_Center).VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("NoSel","选择节点\n预览"))
				.ColorAndOpacity(FLinearColor(0.4f,0.4f,0.4f))
				.Font(FCoreStyle::GetDefaultFontStyle("Italic", 9))
				.Justification(ETextJustify::Center)
			]);
	}
}

TSharedRef<SWidget> SMaterialInheritanceSidebar::BuildPreviewPanel()
{
	return SNew(SBorder)
		.BorderImage(FAppStyle::GetBrush("ToolPanel.DarkGroupBorder"))
		.Padding(FMargin(8, 6))
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot().AutoHeight().Padding(0,0,0,6)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot().FillWidth(1)
				[ SNew(STextBlock).Text(LOCTEXT("PreviewHeader","材质预览")).Font(FCoreStyle::GetDefaultFontStyle("Bold",11)) ]
				+ SHorizontalBox::Slot().AutoWidth()
				[
					SNew(SButton)
					.ButtonStyle(FAppStyle::Get(), "SimpleButton")
					.ToolTipText(LOCTEXT("RefreshPrev","刷新预览"))
					.OnClicked_Lambda([this](){ RefreshPreviewThumbnail(); return FReply::Handled(); })
					.Text(LOCTEXT("RefreshBtn","刷新"))
				]
			]
			// Thumbnail
			+ SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center).Padding(0,2)
			[
				SNew(SBorder).BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
				.Padding(2)
				[
					SAssignNew(ThumbnailBox, SBox).WidthOverride(120).HeightOverride(120)
					.HAlign(HAlign_Center).VAlign(VAlign_Center)
					[
						SNew(STextBlock)
						.Text(LOCTEXT("SelectHint","选择节点"))
						.ColorAndOpacity(FLinearColor(0.4f,0.4f,0.4f))
						.Font(FCoreStyle::GetDefaultFontStyle("Italic",9))
					]
				]
			]
			// Path (on top)
			+ SVerticalBox::Slot().AutoHeight().Padding(0,4,0,0).HAlign(HAlign_Left)
			[ SNew(STextBlock).Text_Lambda([this](){ return OwnerPanel && OwnerPanel->GetSelectedAsset() ? FText::FromString(OwnerPanel->GetSelectedAsset()->GetPathName()) : FText::GetEmpty(); }).ColorAndOpacity(FLinearColor(0.5f,0.5f,0.5f)).Font(FCoreStyle::GetDefaultFontStyle("Regular",7)).AutoWrapText(true) ]
			// Asset name (below, left aligned)
			+ SVerticalBox::Slot().AutoHeight().Padding(0,4,0,0).HAlign(HAlign_Left)
			[ SAssignNew(NameBox, SBox) ]
			
			// Action buttons
			+ SVerticalBox::Slot().AutoHeight().Padding(0,6,0,0)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot().FillWidth(1).Padding(0,0,3,0)
				[ SNew(SButton).Text(LOCTEXT("ViewInh","查看继承")).OnClicked_Lambda([this](){ if(OwnerPanel)OwnerPanel->ViewInheritanceOfSelected(); return FReply::Handled(); }).ToolTipText(LOCTEXT("ViewInhTip","以该材质为中心查看继承关系")) ]
				+ SHorizontalBox::Slot().FillWidth(1).Padding(0,0,3,0)
				[ SNew(SButton).Text(LOCTEXT("EditMat","编辑")).OnClicked_Lambda([this](){ if(OwnerPanel){if(auto* A=OwnerPanel->GetSelectedAsset())GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OpenEditorForAsset(A);} return FReply::Handled(); }).ToolTipText(LOCTEXT("EditTip","在材质编辑器中打开")) ]
				+ SHorizontalBox::Slot().FillWidth(1)
				[ SNew(SButton).Text(LOCTEXT("BrowseTo","浏览")).OnClicked_Lambda([this](){ if(OwnerPanel)OwnerPanel->BrowseSelectedToContentBrowser(); return FReply::Handled(); }).ToolTipText(LOCTEXT("BrowseTip","在内容浏览器中定位")) ]
			]
		];
}

TSharedRef<SWidget> SMaterialInheritanceSidebar::BuildSharedParentTab()
{
	return SNew(SBorder).BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder")).Padding(FMargin(8,6))
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot().AutoHeight().Padding(0,4)
			[ SNew(STextBlock).Text_Lambda([this](){ return FText::Format(LOCTEXT("SelFmt","已选中的子材质实例: {0}"), FText::AsNumber(OwnerPanel?OwnerPanel->GetSelectedChildCount():0)); }).ColorAndOpacity(FLinearColor(0.3f,1.0f,0.3f)).Font(FCoreStyle::GetDefaultFontStyle("Bold",10)) ]
			+ SVerticalBox::Slot().AutoHeight().Padding(0,6,0,0)
			[ SNew(SButton).Text(LOCTEXT("CreateSP","创建共享母材质")).OnClicked_Lambda([this](){ if(OwnerPanel) OwnerPanel->ExecuteCreateSharedParent(); return FReply::Handled(); }) ]
		];
}

TSharedRef<SWidget> SMaterialInheritanceSidebar::BuildBatchRedirectTab()
{
	return SNew(SBorder).BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder")).Padding(FMargin(8,6))
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot().AutoHeight().Padding(0,0,0,6)
			[ SNew(STextBlock).Text(LOCTEXT("BatchRedirectHeader","更改母材质")).Font(FCoreStyle::GetDefaultFontStyle("Bold",11)) ]
			+ SVerticalBox::Slot().AutoHeight().Padding(0,0,0,6)
			[ SNew(STextBlock).Text(LOCTEXT("RedirectDesc","选中至少两个材质，第一个选中的将被设为剩余材质实例的母材质。")).ColorAndOpacity(FLinearColor(0.5f,0.5f,0.5f)).AutoWrapText(true).Font(FCoreStyle::GetDefaultFontStyle("Regular",8)) ]
			+ SVerticalBox::Slot().AutoHeight().Padding(0,6,0,0)
			[ SNew(SButton).Text(LOCTEXT("DoRedirect","更改母材质")).OnClicked_Lambda([this](){ if(OwnerPanel)OwnerPanel->ExecuteChangeParent(); return FReply::Handled(); }) ]
		];
}

TSharedRef<SWidget> SMaterialInheritanceSidebar::BuildAutoMergeTab()
{
	return SNew(SBorder).BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder")).Padding(FMargin(8,6))
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot().AutoHeight().Padding(0,0,0,6)
			[ SNew(STextBlock).Text(LOCTEXT("AutoMergeHeader","自动归并")).Font(FCoreStyle::GetDefaultFontStyle("Bold",11)) ]
			+ SVerticalBox::Slot().AutoHeight().Padding(0,0,0,6)
			[ SNew(STextBlock).Text(LOCTEXT("MergeDesc","自动为拥有享同静态开关参数的材质实例创建共享母材值。")).ColorAndOpacity(FLinearColor(0.5f,0.5f,0.5f)).AutoWrapText(true).Font(FCoreStyle::GetDefaultFontStyle("Regular",8)) ]
			+ SVerticalBox::Slot().AutoHeight().Padding(0,6,0,0)
			[ SNew(SButton).Text(LOCTEXT("DoMerge","执行自动归并")).OnClicked_Lambda([this](){ if(OwnerPanel)OwnerPanel->ExecuteAutoGroupMerge(); return FReply::Handled(); }) ]
		];
}

TSharedRef<SWidget> SMaterialInheritanceSidebar::BuildSettingsTab()
{
	return SNew(SBorder).BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder")).Padding(FMargin(8,6))
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot().AutoHeight().Padding(0,0,0,6) [ SNew(STextBlock).Text(LOCTEXT("SettingsHeader","设置")).Font(FCoreStyle::GetDefaultFontStyle("Bold",11)) ]
			+ SVerticalBox::Slot().AutoHeight().Padding(0,2) [ SNew(STextBlock).Text(LOCTEXT("OutDir","输出目录:")).ColorAndOpacity(FLinearColor(0.5f,0.5f,0.5f)).Font(FCoreStyle::GetDefaultFontStyle("Regular",9)) ]
			+ SVerticalBox::Slot().AutoHeight().Padding(0,0,0,6) [ SNew(SBox).WidthOverride(240) [ SNew(SEditableTextBox).Text_Lambda([this](){ return OwnerPanel?FText::FromString(OwnerPanel->GetOutputDirectory()):FText::GetEmpty(); }).OnTextCommitted_Lambda([this](const FText& T,ETextCommit::Type){ if(OwnerPanel)OwnerPanel->SetOutputDirectory(T.ToString()); }) ]]
			+ SVerticalBox::Slot().AutoHeight().Padding(0,2) [ SNew(STextBlock).Text(LOCTEXT("Depth","展示层级:")).ColorAndOpacity(FLinearColor(0.5f,0.5f,0.5f)).Font(FCoreStyle::GetDefaultFontStyle("Regular",9)) ]
			+ SVerticalBox::Slot().AutoHeight().Padding(0,0,0,6) [ SNew(SBox).WidthOverride(60) [ SNew(SSpinBox<int32>).MinValue(1).MaxValue(10).Value_Lambda([this](){ return OwnerPanel?OwnerPanel->GetSearchDepth():4; }).OnValueCommitted_Lambda([this](int32 V,ETextCommit::Type){ if(OwnerPanel)OwnerPanel->SetSearchDepth(V); }).OnValueChanged_Lambda([this](int32 V){ if(OwnerPanel)OwnerPanel->SetSearchDepthLive(V); }) ]]
			+ SVerticalBox::Slot().AutoHeight().Padding(0,2) [ SNew(STextBlock).Text(LOCTEXT("MaxNodes","最大节点数:")).ColorAndOpacity(FLinearColor(0.5f,0.5f,0.5f)).Font(FCoreStyle::GetDefaultFontStyle("Regular",9)) ]
			+ SVerticalBox::Slot().AutoHeight().Padding(0,0,0,6) [ SNew(SBox).WidthOverride(70) [ SNew(SSpinBox<int32>).MinValue(1).MaxValue(2000).Value_Lambda([this](){ return OwnerPanel?OwnerPanel->GetMaxNodeCount():200; }).OnValueCommitted_Lambda([this](int32 V,ETextCommit::Type){ if(OwnerPanel)OwnerPanel->SetMaxNodeCount(V); }).OnValueChanged_Lambda([this](int32 V){ if(OwnerPanel)OwnerPanel->SetMaxNodeCountLive(V); }) ]]
		];
}

TSharedRef<SWidget> SMaterialInheritanceSidebar::BuildVerificationTab()
{
	return SNew(SBorder).BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder")).Padding(FMargin(8,6))
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot().AutoHeight().Padding(0,0,0,6)
			[ SNew(STextBlock).Text(LOCTEXT("VerifyHeader","参数验证")).Font(FCoreStyle::GetDefaultFontStyle("Bold",11)) ]
			// Directory input
			+ SVerticalBox::Slot().AutoHeight().Padding(0,2)
			[ SNew(STextBlock).Text(LOCTEXT("VerifyDir","目录:")).ColorAndOpacity(FLinearColor(0.5f,0.5f,0.5f)).Font(FCoreStyle::GetDefaultFontStyle("Regular",9)) ]
			+ SVerticalBox::Slot().AutoHeight().Padding(0,0,0,6)
			[ SNew(SBox).WidthOverride(240) [ SAssignNew(VerifyDirInput, SEditableTextBox).Text(FText::FromString(TEXT("/Game/Materials/MergeTest"))) ] ]
			// Buttons
			+ SVerticalBox::Slot().AutoHeight().Padding(0,0,0,4)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot().FillWidth(1).Padding(0,0,3,0)
				[ SNew(SButton).Text(LOCTEXT("RecordSnapshot","记录快照")).OnClicked_Lambda([this]() -> FReply
				{
					FString Dir = VerifyDirInput->GetText().ToString();
					FString Log;
					SnapshotData.Empty();

					FAssetRegistryModule& ARM = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
					IAssetRegistry& AR = ARM.Get();

					FARFilter Filter;
					Filter.bRecursivePaths = true;
					Filter.PackagePaths.Add(FName(*Dir));
					Filter.ClassPaths.Add(UMaterial::StaticClass()->GetClassPathName());
					Filter.ClassPaths.Add(UMaterialInstanceConstant::StaticClass()->GetClassPathName());

					TArray<FAssetData> Assets;
					AR.GetAssets(Filter, Assets);

					for (const FAssetData& AD : Assets)
					{
						UMaterialInterface* Mat = Cast<UMaterialInterface>(AD.GetAsset());
						if (!Mat) continue;
					SnapshotData.Add(FSoftObjectPath(Mat), ReadMaterialVerificationSnapshot(Mat));
					}
					Log = FString::Printf(TEXT("[%s] 快照记录完成。\n共 %d 个材质/MI。"),
						*FDateTime::Now().ToString(), Assets.Num());
					VerificationLog->SetText(FText::FromString(Log));
					return FReply::Handled();
				}) ]
				+ SHorizontalBox::Slot().FillWidth(1)
				[ SNew(SButton).Text(LOCTEXT("CompareSnapshot","对比验证")).OnClicked_Lambda([this]() -> FReply
				{
					FString Dir = VerifyDirInput->GetText().ToString();
					if (SnapshotData.Num() == 0)
					{
						VerificationLog->SetText(LOCTEXT("NoSnapshot","错误：请先点击「记录快照」。"));
						return FReply::Handled();
					}

					FAssetRegistryModule& ARM = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
					IAssetRegistry& AR = ARM.Get();

					FARFilter Filter;
					Filter.bRecursivePaths = true;
					Filter.PackagePaths.Add(FName(*Dir));
					Filter.ClassPaths.Add(UMaterial::StaticClass()->GetClassPathName());
					Filter.ClassPaths.Add(UMaterialInstanceConstant::StaticClass()->GetClassPathName());

					TArray<FAssetData> Assets;
					AR.GetAssets(Filter, Assets);

					TSet<FSoftObjectPath> CurrentPaths;
					TArray<FString> Logs;
					int32 Same = 0, Changed = 0;
					Logs.Add(FString::Printf(TEXT("=== 对比结果  [%s] ==="), *FDateTime::Now().ToString()));

					for (const FAssetData& AD : Assets)
					{
						UMaterialInterface* Mat = Cast<UMaterialInterface>(AD.GetAsset());
						if (!Mat) continue;
						FSoftObjectPath Path(Mat);
						CurrentPaths.Add(Path);

					FMaterialParamSnapshot* OldSnap = SnapshotData.Find(Path);
					if (!OldSnap)
					{
						// 新增的材质不记录日志
						continue;
					}

					FMaterialParamSnapshot NewSnap = ReadMaterialVerificationSnapshot(Mat);

					// 比较有效参数值，并收集逐项的旧值/新值
					FString Diff;
					bool bChanged = CompareMaterialOverrideStates(*OldSnap, NewSnap, Diff);

					// Scalars
					{
						TSet<FName> Keys;
						for (const auto& P : NewSnap.Scalars) Keys.Add(P.Key);
						for (const auto& P : OldSnap->Scalars) Keys.Add(P.Key);
						for (const FName& K : Keys)
						{
							const bool bNew = NewSnap.Scalars.Contains(K);
							const bool bOld = OldSnap->Scalars.Contains(K);
							if (bNew && bOld)
							{
								const float NV = NewSnap.Scalars[K];
								const float OV = OldSnap->Scalars[K];
								if (!FMath::IsNearlyEqual(OV, NV))
								{
									bChanged = true;
									Diff += FString::Printf(TEXT("\n    标量 %s: %s -> %s"), *K.ToString(), *FString::SanitizeFloat(OV), *FString::SanitizeFloat(NV));
								}
							}
							else if (bNew && !bOld)
							{
								bChanged = true;
								Diff += FString::Printf(TEXT("\n    标量 %s: (无) -> %s"), *K.ToString(), *FString::SanitizeFloat(NewSnap.Scalars[K]));
							}
							else
							{
								bChanged = true;
								Diff += FString::Printf(TEXT("\n    标量 %s: %s -> (无)"), *K.ToString(), *FString::SanitizeFloat(OldSnap->Scalars[K]));
							}
						}
					}
					// Vectors
					{
						TSet<FName> Keys;
						for (const auto& P : NewSnap.Vectors) Keys.Add(P.Key);
						for (const auto& P : OldSnap->Vectors) Keys.Add(P.Key);
						for (const FName& K : Keys)
						{
							const bool bNew = NewSnap.Vectors.Contains(K);
							const bool bOld = OldSnap->Vectors.Contains(K);
							if (bNew && bOld)
							{
								if (!NewSnap.Vectors[K].Equals(OldSnap->Vectors[K]))
								{
									bChanged = true;
									Diff += FString::Printf(TEXT("\n    向量 %s: %s -> %s"), *K.ToString(), *OldSnap->Vectors[K].ToString(), *NewSnap.Vectors[K].ToString());
								}
							}
							else if (bNew && !bOld)
							{
								bChanged = true;
								Diff += FString::Printf(TEXT("\n    向量 %s: (无) -> %s"), *K.ToString(), *NewSnap.Vectors[K].ToString());
							}
							else
							{
								bChanged = true;
								Diff += FString::Printf(TEXT("\n    向量 %s: %s -> (无)"), *K.ToString(), *OldSnap->Vectors[K].ToString());
							}
						}
					}
					// Textures (compared by asset path)
					{
						TSet<FName> Keys;
						for (const auto& P : NewSnap.Textures) Keys.Add(P.Key);
						for (const auto& P : OldSnap->Textures) Keys.Add(P.Key);
						for (const FName& K : Keys)
						{
							const bool bNew = NewSnap.Textures.Contains(K);
							const bool bOld = OldSnap->Textures.Contains(K);
							const FString OldName = bOld ? OldSnap->Textures[K].ToSoftObjectPath().ToString() : TEXT("(无)");
							const FString NewName = bNew ? NewSnap.Textures[K].ToSoftObjectPath().ToString() : TEXT("(无)");
							if (bNew && bOld)
							{
								if (OldName != NewName)
								{
									bChanged = true;
									Diff += FString::Printf(TEXT("\n    纹理 %s: %s -> %s"), *K.ToString(), *OldName, *NewName);
								}
							}
							else if (bNew && !bOld)
							{
								bChanged = true;
								Diff += FString::Printf(TEXT("\n    纹理 %s: (无) -> %s"), *K.ToString(), *NewName);
							}
							else
							{
								bChanged = true;
								Diff += FString::Printf(TEXT("\n    纹理 %s: %s -> (无)"), *K.ToString(), *OldName);
							}
						}
					}
					// Static Switches (booleans)
					{
						TSet<FName> Keys;
						for (const auto& P : NewSnap.StaticSwitches.SwitchValues) Keys.Add(P.Key);
						for (const auto& P : OldSnap->StaticSwitches.SwitchValues) Keys.Add(P.Key);
						for (const FName& K : Keys)
						{
							const bool bNew = NewSnap.StaticSwitches.SwitchValues.Contains(K);
							const bool bOld = OldSnap->StaticSwitches.SwitchValues.Contains(K);
							if (bNew && bOld)
							{
								const bool NV = NewSnap.StaticSwitches.SwitchValues[K];
								const bool OV = OldSnap->StaticSwitches.SwitchValues[K];
								if (NV != OV)
								{
									bChanged = true;
									Diff += FString::Printf(TEXT("\n    开关 %s: %s -> %s"), *K.ToString(),
										OV ? TEXT("开") : TEXT("关"), NV ? TEXT("开") : TEXT("关"));
								}
							}
							else if (bNew && !bOld)
							{
								bChanged = true;
								Diff += FString::Printf(TEXT("\n    开关 %s: (无) -> %s"), *K.ToString(),
									NewSnap.StaticSwitches.SwitchValues[K] ? TEXT("开") : TEXT("关"));
							}
							else
							{
								bChanged = true;
								Diff += FString::Printf(TEXT("\n    开关 %s: %s -> (无)"), *K.ToString(),
									OldSnap->StaticSwitches.SwitchValues[K] ? TEXT("开") : TEXT("关"));
							}
						}
					}

					if (bChanged)
					{
						Logs.Add(FString::Printf(TEXT("[变化] %s%s"), *Mat->GetPathName(), *Diff));
						Changed++;
					}
					else
					{
						Same++;
					}
					}

					// Check for deleted
					for (const auto& Pair : SnapshotData)
					{
						if (!CurrentPaths.Contains(Pair.Key))
						{
							Logs.Add(FString::Printf(TEXT("[删除] %s"), *Pair.Key.ToString()));
							Changed++;
						}
					}

					Logs.Add(TEXT("--- 统计 ---"));

					Logs.Add(FString::Printf(TEXT("相同: %d"), Same));
					Logs.Add(FString::Printf(TEXT("变化/新增/删除: %d"), Changed));

					VerificationLog->SetText(FText::FromString(FString::Join(Logs, TEXT("\n"))));
					return FReply::Handled();
				}) ]
			]
			// Log output
			+ SVerticalBox::Slot().AutoHeight().Padding(0,4,0,0)
			[ SNew(STextBlock).Text(LOCTEXT("VerifyLog","日志:")).ColorAndOpacity(FLinearColor(0.5f,0.5f,0.5f)).Font(FCoreStyle::GetDefaultFontStyle("Regular",9)) ]
			+ SVerticalBox::Slot().AutoHeight()
			[
				SAssignNew(VerificationLog, SMultiLineEditableTextBox)
				.IsReadOnly(true)
				.Font(FCoreStyle::GetDefaultFontStyle("Mono", 8))
				.BackgroundColor(FLinearColor(0.02f,0.02f,0.02f))
				.Text(LOCTEXT("VerifyHint","点击「记录快照」保存参数状态，然后「对比验证」检查变化。"))
			]
		];
}


void SMaterialInheritanceSidebar::UpdateNameWidget()
{
	if (!NameBox.IsValid()) return;
	if (bEditingName) return; // 编辑中不重建，避免打断输入/点击
	UObject* Asset = OwnerPanel ? OwnerPanel->GetSelectedAsset() : nullptr;
	FString Name = Asset ? Asset->GetName() : FString();
	// 只有选中资产或名称变化时才重建，避免每帧重建导致按钮实例被销毁、
	// 鼠标按下/抬起落在不同实例上，SButton::OnClicked 永远不触发（无法改名）
	if (LastNameAsset.Get() == Asset && LastNameStr == Name)
	{
		return;
	}
	LastNameAsset = Asset;
	LastNameStr = Name;

	if (!Asset)
	{
		NameBox->SetContent(SNew(STextBlock).Text(LOCTEXT("NoName","请选择节点")).ColorAndOpacity(FLinearColor(0.4f,0.4f,0.4f)).Font(FCoreStyle::GetDefaultFontStyle("Italic",9)));
		return;
	}
		static FSlateColorBrush HoverHighlight(FLinearColor(0.18f, 0.18f, 0.22f, 1.0f));
		static FButtonStyle NameBtnStyle = FButtonStyle()
			.SetNormal(FSlateNoResource())
			.SetHovered(HoverHighlight)
			.SetPressed(FSlateNoResource())
			.SetNormalPadding(FMargin(0))
			.SetPressedPadding(FMargin(0));
		TSharedRef<SButton> NameBtn = SNew(SButton)
			.ButtonStyle(&NameBtnStyle)
			.ContentPadding(FMargin(4, 2))
			.ToolTipText(LOCTEXT("RenameTip","点击这里改名"))
			.OnClicked(this, &SMaterialInheritanceSidebar::OnNameClicked)
			[
				SNew(STextBlock)
				.Text(FText::FromString(Asset->GetName()))
				.ColorAndOpacity(FLinearColor(0.9f,0.9f,0.9f))
				.Font(FCoreStyle::GetDefaultFontStyle("Bold",10))
			];
		NameBox->SetContent(NameBtn);
}

FReply SMaterialInheritanceSidebar::OnNameClicked()
{
	UObject* Asset = OwnerPanel ? OwnerPanel->GetSelectedAsset() : nullptr;
	if (!Asset || bEditingName) return FReply::Handled();
	bEditingName = true;
	TSharedPtr<SEditableTextBox> EditBox;
	NameBox->SetContent(
		SAssignNew(EditBox, SEditableTextBox)
		.Text(FText::FromString(Asset->GetName()))
		.Font(FCoreStyle::GetDefaultFontStyle("Bold",10))
		.SelectAllTextWhenFocused(true)
		.RevertTextOnEscape(true)
		.OnTextCommitted(this, &SMaterialInheritanceSidebar::OnNameCommitted)
	);
	FSlateApplication::Get().SetKeyboardFocus(EditBox);
	return FReply::Handled();
}

void SMaterialInheritanceSidebar::OnNameCommitted(const FText& NewText, ETextCommit::Type CommitType)
{
	bEditingName = false;
	FString NewName = NewText.ToString();
	if (OwnerPanel && !NewName.IsEmpty())
	{
		OwnerPanel->RenameSelectedAsset(NewName);
	}
	UpdateNameWidget();
}

#undef LOCTEXT_NAMESPACE
