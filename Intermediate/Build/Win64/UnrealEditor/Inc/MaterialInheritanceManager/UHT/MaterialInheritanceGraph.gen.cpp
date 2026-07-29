// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "MaterialInheritanceGraph.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
static_assert(!UE_WITH_CONSTINIT_UOBJECT, "This generated code can only be compiled with !UE_WITH_CONSTINIT_OBJECT");
void EmptyLinkFunctionForGeneratedCodeMaterialInheritanceGraph() {}

// ********** Begin Cross Module References ********************************************************
ENGINE_API UClass* Z_Construct_UClass_UEdGraph();
MATERIALINHERITANCEMANAGER_API UClass* Z_Construct_UClass_UMaterialInheritanceGraph();
MATERIALINHERITANCEMANAGER_API UClass* Z_Construct_UClass_UMaterialInheritanceGraph_NoRegister();
UPackage* Z_Construct_UPackage__Script_MaterialInheritanceManager();
// ********** End Cross Module References **********************************************************

// ********** Begin Class UMaterialInheritanceGraph ************************************************
FClassRegistrationInfo Z_Registration_Info_UClass_UMaterialInheritanceGraph;
UClass* UMaterialInheritanceGraph::GetPrivateStaticClass()
{
	using TClass = UMaterialInheritanceGraph;
	if (!Z_Registration_Info_UClass_UMaterialInheritanceGraph.InnerSingleton)
	{
		GetPrivateStaticClassBody(
			TClass::StaticPackage(),
			TEXT("MaterialInheritanceGraph"),
			Z_Registration_Info_UClass_UMaterialInheritanceGraph.InnerSingleton,
			StaticRegisterNativesUMaterialInheritanceGraph,
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
	return Z_Registration_Info_UClass_UMaterialInheritanceGraph.InnerSingleton;
}
UClass* Z_Construct_UClass_UMaterialInheritanceGraph_NoRegister()
{
	return UMaterialInheritanceGraph::GetPrivateStaticClass();
}
struct Z_Construct_UClass_UMaterialInheritanceGraph_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "IncludePath", "MaterialInheritanceGraph.h" },
		{ "ModuleRelativePath", "Public/MaterialInheritanceGraph.h" },
	};
#endif // WITH_METADATA

// ********** Begin Class UMaterialInheritanceGraph constinit property declarations ****************
// ********** End Class UMaterialInheritanceGraph constinit property declarations ******************
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UMaterialInheritanceGraph>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
}; // struct Z_Construct_UClass_UMaterialInheritanceGraph_Statics
UObject* (*const Z_Construct_UClass_UMaterialInheritanceGraph_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_UEdGraph,
	(UObject* (*)())Z_Construct_UPackage__Script_MaterialInheritanceManager,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UMaterialInheritanceGraph_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_UMaterialInheritanceGraph_Statics::ClassParams = {
	&UMaterialInheritanceGraph::StaticClass,
	nullptr,
	&StaticCppClassTypeInfo,
	DependentSingletons,
	nullptr,
	nullptr,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	0,
	0,
	0,
	0x000000A0u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UMaterialInheritanceGraph_Statics::Class_MetaDataParams), Z_Construct_UClass_UMaterialInheritanceGraph_Statics::Class_MetaDataParams)
};
void UMaterialInheritanceGraph::StaticRegisterNativesUMaterialInheritanceGraph()
{
}
UClass* Z_Construct_UClass_UMaterialInheritanceGraph()
{
	if (!Z_Registration_Info_UClass_UMaterialInheritanceGraph.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UMaterialInheritanceGraph.OuterSingleton, Z_Construct_UClass_UMaterialInheritanceGraph_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_UMaterialInheritanceGraph.OuterSingleton;
}
UMaterialInheritanceGraph::UMaterialInheritanceGraph(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {}
DEFINE_VTABLE_PTR_HELPER_CTOR_NS(, UMaterialInheritanceGraph);
UMaterialInheritanceGraph::~UMaterialInheritanceGraph() {}
// ********** End Class UMaterialInheritanceGraph **************************************************

// ********** Begin Registration *******************************************************************
struct Z_CompiledInDeferFile_FID_Users_tairuili_Documents_Unreal_Projects_MaterialTest2_Plugins_MaterialInheritanceManager_Source_MaterialInheritanceManager_Public_MaterialInheritanceGraph_h__Script_MaterialInheritanceManager_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_UMaterialInheritanceGraph, UMaterialInheritanceGraph::StaticClass, TEXT("UMaterialInheritanceGraph"), &Z_Registration_Info_UClass_UMaterialInheritanceGraph, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UMaterialInheritanceGraph), 1922401846U) },
	};
}; // Z_CompiledInDeferFile_FID_Users_tairuili_Documents_Unreal_Projects_MaterialTest2_Plugins_MaterialInheritanceManager_Source_MaterialInheritanceManager_Public_MaterialInheritanceGraph_h__Script_MaterialInheritanceManager_Statics 
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_tairuili_Documents_Unreal_Projects_MaterialTest2_Plugins_MaterialInheritanceManager_Source_MaterialInheritanceManager_Public_MaterialInheritanceGraph_h__Script_MaterialInheritanceManager_265673259{
	TEXT("/Script/MaterialInheritanceManager"),
	Z_CompiledInDeferFile_FID_Users_tairuili_Documents_Unreal_Projects_MaterialTest2_Plugins_MaterialInheritanceManager_Source_MaterialInheritanceManager_Public_MaterialInheritanceGraph_h__Script_MaterialInheritanceManager_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_tairuili_Documents_Unreal_Projects_MaterialTest2_Plugins_MaterialInheritanceManager_Source_MaterialInheritanceManager_Public_MaterialInheritanceGraph_h__Script_MaterialInheritanceManager_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0,
};
// ********** End Registration *********************************************************************

PRAGMA_ENABLE_DEPRECATION_WARNINGS
