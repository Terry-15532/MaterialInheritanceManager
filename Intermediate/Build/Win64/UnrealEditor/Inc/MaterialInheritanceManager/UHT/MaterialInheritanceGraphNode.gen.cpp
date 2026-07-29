// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "MaterialInheritanceGraphNode.h"
#include "UObject/SoftObjectPath.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
static_assert(!UE_WITH_CONSTINIT_UOBJECT, "This generated code can only be compiled with !UE_WITH_CONSTINIT_OBJECT");
void EmptyLinkFunctionForGeneratedCodeMaterialInheritanceGraphNode() {}

// ********** Begin Cross Module References ********************************************************
COREUOBJECT_API UScriptStruct* Z_Construct_UScriptStruct_FSoftObjectPath();
ENGINE_API UClass* Z_Construct_UClass_UEdGraphNode();
ENGINE_API UClass* Z_Construct_UClass_UMaterialInterface_NoRegister();
MATERIALINHERITANCEMANAGER_API UClass* Z_Construct_UClass_UMaterialInheritanceGraphNode();
MATERIALINHERITANCEMANAGER_API UClass* Z_Construct_UClass_UMaterialInheritanceGraphNode_NoRegister();
UPackage* Z_Construct_UPackage__Script_MaterialInheritanceManager();
// ********** End Cross Module References **********************************************************

// ********** Begin Class UMaterialInheritanceGraphNode Function GetMaterialAsset ******************
struct Z_Construct_UFunction_UMaterialInheritanceGraphNode_GetMaterialAsset_Statics
{
	struct MaterialInheritanceGraphNode_eventGetMaterialAsset_Parms
	{
		UMaterialInterface* ReturnValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "Category", "Material Inheritance" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// \xe6\x8c\x89\xe9\x9c\x80\xe5\x8a\xa0\xe8\xbd\xbd\xe5\xba\x95\xe5\xb1\x82\xe6\x9d\x90\xe8\xb4\xa8/\xe6\x9d\x90\xe8\xb4\xa8\xe5\xae\x9e\xe4\xbe\x8b\xef\xbc\x8c\xe5\xb9\xb6\xe7\xbc\x93\xe5\xad\x98\xe7\xbb\x93\xe6\x9e\x9c\xe3\x80\x82\xe8\xbf\x94\xe5\x9b\x9e null \xe8\xa1\xa8\xe7\xa4\xba\xe5\x8a\xa0\xe8\xbd\xbd\xe5\xa4\xb1\xe8\xb4\xa5\xe3\x80\x82\n" },
#endif
		{ "ModuleRelativePath", "Public/MaterialInheritanceGraphNode.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "\xe6\x8c\x89\xe9\x9c\x80\xe5\x8a\xa0\xe8\xbd\xbd\xe5\xba\x95\xe5\xb1\x82\xe6\x9d\x90\xe8\xb4\xa8/\xe6\x9d\x90\xe8\xb4\xa8\xe5\xae\x9e\xe4\xbe\x8b\xef\xbc\x8c\xe5\xb9\xb6\xe7\xbc\x93\xe5\xad\x98\xe7\xbb\x93\xe6\x9e\x9c\xe3\x80\x82\xe8\xbf\x94\xe5\x9b\x9e null \xe8\xa1\xa8\xe7\xa4\xba\xe5\x8a\xa0\xe8\xbd\xbd\xe5\xa4\xb1\xe8\xb4\xa5\xe3\x80\x82" },
#endif
	};
#endif // WITH_METADATA

// ********** Begin Function GetMaterialAsset constinit property declarations **********************
	static const UECodeGen_Private::FObjectPropertyParams NewProp_ReturnValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Function GetMaterialAsset constinit property declarations ************************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};

// ********** Begin Function GetMaterialAsset Property Definitions *********************************
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UFunction_UMaterialInheritanceGraphNode_GetMaterialAsset_Statics::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000000580, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(MaterialInheritanceGraphNode_eventGetMaterialAsset_Parms, ReturnValue), Z_Construct_UClass_UMaterialInterface_NoRegister, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UMaterialInheritanceGraphNode_GetMaterialAsset_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UMaterialInheritanceGraphNode_GetMaterialAsset_Statics::NewProp_ReturnValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UMaterialInheritanceGraphNode_GetMaterialAsset_Statics::PropPointers) < 2048);
// ********** End Function GetMaterialAsset Property Definitions ***********************************
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UMaterialInheritanceGraphNode_GetMaterialAsset_Statics::FuncParams = { { (UObject*(*)())Z_Construct_UClass_UMaterialInheritanceGraphNode, nullptr, "GetMaterialAsset", 	Z_Construct_UFunction_UMaterialInheritanceGraphNode_GetMaterialAsset_Statics::PropPointers, 
	UE_ARRAY_COUNT(Z_Construct_UFunction_UMaterialInheritanceGraphNode_GetMaterialAsset_Statics::PropPointers), 
sizeof(Z_Construct_UFunction_UMaterialInheritanceGraphNode_GetMaterialAsset_Statics::MaterialInheritanceGraphNode_eventGetMaterialAsset_Parms),
RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04020401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UMaterialInheritanceGraphNode_GetMaterialAsset_Statics::Function_MetaDataParams), Z_Construct_UFunction_UMaterialInheritanceGraphNode_GetMaterialAsset_Statics::Function_MetaDataParams)},  };
static_assert(sizeof(Z_Construct_UFunction_UMaterialInheritanceGraphNode_GetMaterialAsset_Statics::MaterialInheritanceGraphNode_eventGetMaterialAsset_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UMaterialInheritanceGraphNode_GetMaterialAsset()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UMaterialInheritanceGraphNode_GetMaterialAsset_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(UMaterialInheritanceGraphNode::execGetMaterialAsset)
{
	P_FINISH;
	P_NATIVE_BEGIN;
	*(UMaterialInterface**)Z_Param__Result=P_THIS->GetMaterialAsset();
	P_NATIVE_END;
}
// ********** End Class UMaterialInheritanceGraphNode Function GetMaterialAsset ********************

// ********** Begin Class UMaterialInheritanceGraphNode ********************************************
FClassRegistrationInfo Z_Registration_Info_UClass_UMaterialInheritanceGraphNode;
UClass* UMaterialInheritanceGraphNode::GetPrivateStaticClass()
{
	using TClass = UMaterialInheritanceGraphNode;
	if (!Z_Registration_Info_UClass_UMaterialInheritanceGraphNode.InnerSingleton)
	{
		GetPrivateStaticClassBody(
			TClass::StaticPackage(),
			TEXT("MaterialInheritanceGraphNode"),
			Z_Registration_Info_UClass_UMaterialInheritanceGraphNode.InnerSingleton,
			StaticRegisterNativesUMaterialInheritanceGraphNode,
			sizeof(TClass),
			alignof(TClass),
			TClass::StaticClassFlags,
			TClass::StaticClassCastFlags(),
			TClass::StaticConfigName(),
			(UClass::ClassConstructorType)InternalConstructor<TClass>,
			(UClass::ClassVTableHelperCtorCallerType)InternalVTableHelperCtorCaller<TClass>,
			UOBJECT_CPPCLASS_STATICFUNCTIONS_FORCLASS(TClass),
			&TClass::Super::StaticClass,
			&TClass::WithinClass::StaticClass
		);
	}
	return Z_Registration_Info_UClass_UMaterialInheritanceGraphNode.InnerSingleton;
}
UClass* Z_Construct_UClass_UMaterialInheritanceGraphNode_NoRegister()
{
	return UMaterialInheritanceGraphNode::GetPrivateStaticClass();
}
struct Z_Construct_UClass_UMaterialInheritanceGraphNode_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "IncludePath", "MaterialInheritanceGraphNode.h" },
		{ "ModuleRelativePath", "Public/MaterialInheritanceGraphNode.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_MaterialAsset_MetaData[] = {
		{ "ModuleRelativePath", "Public/MaterialInheritanceGraphNode.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_AssetPath_MetaData[] = {
#if !UE_BUILD_SHIPPING
		{ "Comment", "// \xe6\x9e\x84\xe5\xbb\xba\xe6\x97\xb6\xe5\x8f\xaa\xe4\xbf\x9d\xe5\xad\x98\xe8\xb7\xaf\xe5\xbe\x84\xe4\xb8\x8e\xe5\x90\x8d\xe7\xa7\xb0\xef\xbc\x8c\xe9\x81\xbf\xe5\x85\x8d\xe4\xb8\x80\xe6\xac\xa1\xe6\x80\xa7\xe4\xbb\x8e\xe7\xa3\x81\xe7\x9b\x98\xe5\x8a\xa0\xe8\xbd\xbd\xe6\x88\x90\xe7\x99\xbe\xe4\xb8\x8a\xe5\x8d\x83\xe4\xb8\xaa\xe8\xb5\x84\xe4\xba\xa7\xe3\x80\x82\n// \xe7\x9c\x9f\xe5\xae\x9e\xe8\xb5\x84\xe4\xba\xa7\xe5\x9c\xa8\xe7\x94\xa8\xe6\x88\xb7\xe7\x82\xb9\xe5\x87\xbb\xe6\x88\x96\xe6\x82\xac\xe6\xb5\xae\xe6\x97\xb6\xe6\x89\x8d\xe6\x8c\x89\xe9\x9c\x80\xe5\x8a\xa0\xe8\xbd\xbd\xef\xbc\x88\xe8\xa7\x81 GetMaterialAsset\xef\xbc\x89\xe3\x80\x82\n" },
#endif
		{ "ModuleRelativePath", "Public/MaterialInheritanceGraphNode.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "\xe6\x9e\x84\xe5\xbb\xba\xe6\x97\xb6\xe5\x8f\xaa\xe4\xbf\x9d\xe5\xad\x98\xe8\xb7\xaf\xe5\xbe\x84\xe4\xb8\x8e\xe5\x90\x8d\xe7\xa7\xb0\xef\xbc\x8c\xe9\x81\xbf\xe5\x85\x8d\xe4\xb8\x80\xe6\xac\xa1\xe6\x80\xa7\xe4\xbb\x8e\xe7\xa3\x81\xe7\x9b\x98\xe5\x8a\xa0\xe8\xbd\xbd\xe6\x88\x90\xe7\x99\xbe\xe4\xb8\x8a\xe5\x8d\x83\xe4\xb8\xaa\xe8\xb5\x84\xe4\xba\xa7\xe3\x80\x82\n\xe7\x9c\x9f\xe5\xae\x9e\xe8\xb5\x84\xe4\xba\xa7\xe5\x9c\xa8\xe7\x94\xa8\xe6\x88\xb7\xe7\x82\xb9\xe5\x87\xbb\xe6\x88\x96\xe6\x82\xac\xe6\xb5\xae\xe6\x97\xb6\xe6\x89\x8d\xe6\x8c\x89\xe9\x9c\x80\xe5\x8a\xa0\xe8\xbd\xbd\xef\xbc\x88\xe8\xa7\x81 GetMaterialAsset\xef\xbc\x89\xe3\x80\x82" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_CachedName_MetaData[] = {
		{ "ModuleRelativePath", "Public/MaterialInheritanceGraphNode.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_SwitchSummary_MetaData[] = {
		{ "ModuleRelativePath", "Public/MaterialInheritanceGraphNode.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_bIsBaseMaterial_MetaData[] = {
		{ "ModuleRelativePath", "Public/MaterialInheritanceGraphNode.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_bIsCurrent_MetaData[] = {
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** True when this node is the material currently being viewed. */" },
#endif
		{ "ModuleRelativePath", "Public/MaterialInheritanceGraphNode.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "True when this node is the material currently being viewed." },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Depth_MetaData[] = {
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** 0 = selected, negative = ancestor, positive = descendant */" },
#endif
		{ "ModuleRelativePath", "Public/MaterialInheritanceGraphNode.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "0 = selected, negative = ancestor, positive = descendant" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_bIsCollapsedPlaceholder_MetaData[] = {
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** True if this is the \"...\" collapsed placeholder node. */" },
#endif
		{ "ModuleRelativePath", "Public/MaterialInheritanceGraphNode.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "True if this is the \"...\" collapsed placeholder node." },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_bIsDepthCollapse_MetaData[] = {
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** True when the placeholder stands for hidden DEEPER levels (expand increases depth);\n\x09 *  false when it stands for nodes hidden by the node-count limit (expand increases count). */" },
#endif
		{ "ModuleRelativePath", "Public/MaterialInheritanceGraphNode.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "True when the placeholder stands for hidden DEEPER levels (expand increases depth);\nfalse when it stands for nodes hidden by the node-count limit (expand increases count)." },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_CollapsedParentPath_MetaData[] = {
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** The parent asset that this \"...\" node should expand. */" },
#endif
		{ "ModuleRelativePath", "Public/MaterialInheritanceGraphNode.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "The parent asset that this \"...\" node should expand." },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_CollapsedHiddenCount_MetaData[] = {
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Number of child items hidden behind this placeholder (for tooltip display). */" },
#endif
		{ "ModuleRelativePath", "Public/MaterialInheritanceGraphNode.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Number of child items hidden behind this placeholder (for tooltip display)." },
#endif
	};
#endif // WITH_METADATA

// ********** Begin Class UMaterialInheritanceGraphNode constinit property declarations ************
	static const UECodeGen_Private::FObjectPropertyParams NewProp_MaterialAsset;
	static const UECodeGen_Private::FStructPropertyParams NewProp_AssetPath;
	static const UECodeGen_Private::FStrPropertyParams NewProp_CachedName;
	static const UECodeGen_Private::FStrPropertyParams NewProp_SwitchSummary;
	static void NewProp_bIsBaseMaterial_SetBit(void* Obj);
	static const UECodeGen_Private::FBoolPropertyParams NewProp_bIsBaseMaterial;
	static void NewProp_bIsCurrent_SetBit(void* Obj);
	static const UECodeGen_Private::FBoolPropertyParams NewProp_bIsCurrent;
	static const UECodeGen_Private::FIntPropertyParams NewProp_Depth;
	static void NewProp_bIsCollapsedPlaceholder_SetBit(void* Obj);
	static const UECodeGen_Private::FBoolPropertyParams NewProp_bIsCollapsedPlaceholder;
	static void NewProp_bIsDepthCollapse_SetBit(void* Obj);
	static const UECodeGen_Private::FBoolPropertyParams NewProp_bIsDepthCollapse;
	static const UECodeGen_Private::FStructPropertyParams NewProp_CollapsedParentPath;
	static const UECodeGen_Private::FIntPropertyParams NewProp_CollapsedHiddenCount;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Class UMaterialInheritanceGraphNode constinit property declarations **************
	static constexpr UE::CodeGen::FClassNativeFunction Funcs[] = {
		{ .NameUTF8 = UTF8TEXT("GetMaterialAsset"), .Pointer = &UMaterialInheritanceGraphNode::execGetMaterialAsset },
	};
	static UObject* (*const DependentSingletons[])();
	static constexpr FClassFunctionLinkInfo FuncInfo[] = {
		{ &Z_Construct_UFunction_UMaterialInheritanceGraphNode_GetMaterialAsset, "GetMaterialAsset" }, // 1641506702
	};
	static_assert(UE_ARRAY_COUNT(FuncInfo) < 2048);
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UMaterialInheritanceGraphNode>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
}; // struct Z_Construct_UClass_UMaterialInheritanceGraphNode_Statics

// ********** Begin Class UMaterialInheritanceGraphNode Property Definitions ***********************
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_UMaterialInheritanceGraphNode_Statics::NewProp_MaterialAsset = { "MaterialAsset", nullptr, (EPropertyFlags)0x0114000000000000, UECodeGen_Private::EPropertyGenFlags::Object | UECodeGen_Private::EPropertyGenFlags::ObjectPtr, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UMaterialInheritanceGraphNode, MaterialAsset), Z_Construct_UClass_UMaterialInterface_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_MaterialAsset_MetaData), NewProp_MaterialAsset_MetaData) };
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UMaterialInheritanceGraphNode_Statics::NewProp_AssetPath = { "AssetPath", nullptr, (EPropertyFlags)0x0010000000000000, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UMaterialInheritanceGraphNode, AssetPath), Z_Construct_UScriptStruct_FSoftObjectPath, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_AssetPath_MetaData), NewProp_AssetPath_MetaData) }; // 2425717601
const UECodeGen_Private::FStrPropertyParams Z_Construct_UClass_UMaterialInheritanceGraphNode_Statics::NewProp_CachedName = { "CachedName", nullptr, (EPropertyFlags)0x0010000000000000, UECodeGen_Private::EPropertyGenFlags::Str, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UMaterialInheritanceGraphNode, CachedName), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_CachedName_MetaData), NewProp_CachedName_MetaData) };
const UECodeGen_Private::FStrPropertyParams Z_Construct_UClass_UMaterialInheritanceGraphNode_Statics::NewProp_SwitchSummary = { "SwitchSummary", nullptr, (EPropertyFlags)0x0010000000000000, UECodeGen_Private::EPropertyGenFlags::Str, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UMaterialInheritanceGraphNode, SwitchSummary), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_SwitchSummary_MetaData), NewProp_SwitchSummary_MetaData) };
void Z_Construct_UClass_UMaterialInheritanceGraphNode_Statics::NewProp_bIsBaseMaterial_SetBit(void* Obj)
{
	((UMaterialInheritanceGraphNode*)Obj)->bIsBaseMaterial = 1;
}
const UECodeGen_Private::FBoolPropertyParams Z_Construct_UClass_UMaterialInheritanceGraphNode_Statics::NewProp_bIsBaseMaterial = { "bIsBaseMaterial", nullptr, (EPropertyFlags)0x0010000000000000, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(UMaterialInheritanceGraphNode), &Z_Construct_UClass_UMaterialInheritanceGraphNode_Statics::NewProp_bIsBaseMaterial_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_bIsBaseMaterial_MetaData), NewProp_bIsBaseMaterial_MetaData) };
void Z_Construct_UClass_UMaterialInheritanceGraphNode_Statics::NewProp_bIsCurrent_SetBit(void* Obj)
{
	((UMaterialInheritanceGraphNode*)Obj)->bIsCurrent = 1;
}
const UECodeGen_Private::FBoolPropertyParams Z_Construct_UClass_UMaterialInheritanceGraphNode_Statics::NewProp_bIsCurrent = { "bIsCurrent", nullptr, (EPropertyFlags)0x0010000000000000, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(UMaterialInheritanceGraphNode), &Z_Construct_UClass_UMaterialInheritanceGraphNode_Statics::NewProp_bIsCurrent_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_bIsCurrent_MetaData), NewProp_bIsCurrent_MetaData) };
const UECodeGen_Private::FIntPropertyParams Z_Construct_UClass_UMaterialInheritanceGraphNode_Statics::NewProp_Depth = { "Depth", nullptr, (EPropertyFlags)0x0010000000000000, UECodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UMaterialInheritanceGraphNode, Depth), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Depth_MetaData), NewProp_Depth_MetaData) };
void Z_Construct_UClass_UMaterialInheritanceGraphNode_Statics::NewProp_bIsCollapsedPlaceholder_SetBit(void* Obj)
{
	((UMaterialInheritanceGraphNode*)Obj)->bIsCollapsedPlaceholder = 1;
}
const UECodeGen_Private::FBoolPropertyParams Z_Construct_UClass_UMaterialInheritanceGraphNode_Statics::NewProp_bIsCollapsedPlaceholder = { "bIsCollapsedPlaceholder", nullptr, (EPropertyFlags)0x0010000000000000, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(UMaterialInheritanceGraphNode), &Z_Construct_UClass_UMaterialInheritanceGraphNode_Statics::NewProp_bIsCollapsedPlaceholder_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_bIsCollapsedPlaceholder_MetaData), NewProp_bIsCollapsedPlaceholder_MetaData) };
void Z_Construct_UClass_UMaterialInheritanceGraphNode_Statics::NewProp_bIsDepthCollapse_SetBit(void* Obj)
{
	((UMaterialInheritanceGraphNode*)Obj)->bIsDepthCollapse = 1;
}
const UECodeGen_Private::FBoolPropertyParams Z_Construct_UClass_UMaterialInheritanceGraphNode_Statics::NewProp_bIsDepthCollapse = { "bIsDepthCollapse", nullptr, (EPropertyFlags)0x0010000000000000, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(UMaterialInheritanceGraphNode), &Z_Construct_UClass_UMaterialInheritanceGraphNode_Statics::NewProp_bIsDepthCollapse_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_bIsDepthCollapse_MetaData), NewProp_bIsDepthCollapse_MetaData) };
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UMaterialInheritanceGraphNode_Statics::NewProp_CollapsedParentPath = { "CollapsedParentPath", nullptr, (EPropertyFlags)0x0010000000000000, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UMaterialInheritanceGraphNode, CollapsedParentPath), Z_Construct_UScriptStruct_FSoftObjectPath, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_CollapsedParentPath_MetaData), NewProp_CollapsedParentPath_MetaData) }; // 2425717601
const UECodeGen_Private::FIntPropertyParams Z_Construct_UClass_UMaterialInheritanceGraphNode_Statics::NewProp_CollapsedHiddenCount = { "CollapsedHiddenCount", nullptr, (EPropertyFlags)0x0010000000000000, UECodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UMaterialInheritanceGraphNode, CollapsedHiddenCount), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_CollapsedHiddenCount_MetaData), NewProp_CollapsedHiddenCount_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UMaterialInheritanceGraphNode_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UMaterialInheritanceGraphNode_Statics::NewProp_MaterialAsset,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UMaterialInheritanceGraphNode_Statics::NewProp_AssetPath,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UMaterialInheritanceGraphNode_Statics::NewProp_CachedName,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UMaterialInheritanceGraphNode_Statics::NewProp_SwitchSummary,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UMaterialInheritanceGraphNode_Statics::NewProp_bIsBaseMaterial,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UMaterialInheritanceGraphNode_Statics::NewProp_bIsCurrent,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UMaterialInheritanceGraphNode_Statics::NewProp_Depth,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UMaterialInheritanceGraphNode_Statics::NewProp_bIsCollapsedPlaceholder,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UMaterialInheritanceGraphNode_Statics::NewProp_bIsDepthCollapse,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UMaterialInheritanceGraphNode_Statics::NewProp_CollapsedParentPath,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UMaterialInheritanceGraphNode_Statics::NewProp_CollapsedHiddenCount,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UMaterialInheritanceGraphNode_Statics::PropPointers) < 2048);
// ********** End Class UMaterialInheritanceGraphNode Property Definitions *************************
UObject* (*const Z_Construct_UClass_UMaterialInheritanceGraphNode_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_UEdGraphNode,
	(UObject* (*)())Z_Construct_UPackage__Script_MaterialInheritanceManager,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UMaterialInheritanceGraphNode_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_UMaterialInheritanceGraphNode_Statics::ClassParams = {
	&UMaterialInheritanceGraphNode::StaticClass,
	nullptr,
	&StaticCppClassTypeInfo,
	DependentSingletons,
	FuncInfo,
	Z_Construct_UClass_UMaterialInheritanceGraphNode_Statics::PropPointers,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	UE_ARRAY_COUNT(FuncInfo),
	UE_ARRAY_COUNT(Z_Construct_UClass_UMaterialInheritanceGraphNode_Statics::PropPointers),
	0,
	0x000000A0u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UMaterialInheritanceGraphNode_Statics::Class_MetaDataParams), Z_Construct_UClass_UMaterialInheritanceGraphNode_Statics::Class_MetaDataParams)
};
void UMaterialInheritanceGraphNode::StaticRegisterNativesUMaterialInheritanceGraphNode()
{
	UClass* Class = UMaterialInheritanceGraphNode::StaticClass();
	FNativeFunctionRegistrar::RegisterFunctions(Class, MakeConstArrayView(Z_Construct_UClass_UMaterialInheritanceGraphNode_Statics::Funcs));
}
UClass* Z_Construct_UClass_UMaterialInheritanceGraphNode()
{
	if (!Z_Registration_Info_UClass_UMaterialInheritanceGraphNode.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UMaterialInheritanceGraphNode.OuterSingleton, Z_Construct_UClass_UMaterialInheritanceGraphNode_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_UMaterialInheritanceGraphNode.OuterSingleton;
}
UMaterialInheritanceGraphNode::UMaterialInheritanceGraphNode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {}
DEFINE_VTABLE_PTR_HELPER_CTOR_NS(, UMaterialInheritanceGraphNode);
UMaterialInheritanceGraphNode::~UMaterialInheritanceGraphNode() {}
// ********** End Class UMaterialInheritanceGraphNode **********************************************

// ********** Begin Registration *******************************************************************
struct Z_CompiledInDeferFile_FID_Users_tairuili_Documents_Unreal_Projects_MaterialTest2_Plugins_MaterialInheritanceManager_Source_MaterialInheritanceManager_Public_MaterialInheritanceGraphNode_h__Script_MaterialInheritanceManager_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_UMaterialInheritanceGraphNode, UMaterialInheritanceGraphNode::StaticClass, TEXT("UMaterialInheritanceGraphNode"), &Z_Registration_Info_UClass_UMaterialInheritanceGraphNode, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UMaterialInheritanceGraphNode), 1925574064U) },
	};
}; // Z_CompiledInDeferFile_FID_Users_tairuili_Documents_Unreal_Projects_MaterialTest2_Plugins_MaterialInheritanceManager_Source_MaterialInheritanceManager_Public_MaterialInheritanceGraphNode_h__Script_MaterialInheritanceManager_Statics 
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_tairuili_Documents_Unreal_Projects_MaterialTest2_Plugins_MaterialInheritanceManager_Source_MaterialInheritanceManager_Public_MaterialInheritanceGraphNode_h__Script_MaterialInheritanceManager_3379365934{
	TEXT("/Script/MaterialInheritanceManager"),
	Z_CompiledInDeferFile_FID_Users_tairuili_Documents_Unreal_Projects_MaterialTest2_Plugins_MaterialInheritanceManager_Source_MaterialInheritanceManager_Public_MaterialInheritanceGraphNode_h__Script_MaterialInheritanceManager_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_tairuili_Documents_Unreal_Projects_MaterialTest2_Plugins_MaterialInheritanceManager_Source_MaterialInheritanceManager_Public_MaterialInheritanceGraphNode_h__Script_MaterialInheritanceManager_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0,
};
// ********** End Registration *********************************************************************

PRAGMA_ENABLE_DEPRECATION_WARNINGS
