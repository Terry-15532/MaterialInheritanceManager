// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "MaterialInheritanceManagerConfig.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
static_assert(!UE_WITH_CONSTINIT_UOBJECT, "This generated code can only be compiled with !UE_WITH_CONSTINIT_OBJECT");
void EmptyLinkFunctionForGeneratedCodeMaterialInheritanceManagerConfig() {}

// ********** Begin Cross Module References ********************************************************
COREUOBJECT_API UClass* Z_Construct_UClass_UObject();
MATERIALINHERITANCEMANAGER_API UClass* Z_Construct_UClass_UMaterialInheritanceManagerConfig();
MATERIALINHERITANCEMANAGER_API UClass* Z_Construct_UClass_UMaterialInheritanceManagerConfig_NoRegister();
UPackage* Z_Construct_UPackage__Script_MaterialInheritanceManager();
// ********** End Cross Module References **********************************************************

// ********** Begin Class UMaterialInheritanceManagerConfig ****************************************
FClassRegistrationInfo Z_Registration_Info_UClass_UMaterialInheritanceManagerConfig;
UClass* UMaterialInheritanceManagerConfig::GetPrivateStaticClass()
{
	using TClass = UMaterialInheritanceManagerConfig;
	if (!Z_Registration_Info_UClass_UMaterialInheritanceManagerConfig.InnerSingleton)
	{
		GetPrivateStaticClassBody(
			TClass::StaticPackage(),
			TEXT("MaterialInheritanceManagerConfig"),
			Z_Registration_Info_UClass_UMaterialInheritanceManagerConfig.InnerSingleton,
			StaticRegisterNativesUMaterialInheritanceManagerConfig,
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
	return Z_Registration_Info_UClass_UMaterialInheritanceManagerConfig.InnerSingleton;
}
UClass* Z_Construct_UClass_UMaterialInheritanceManagerConfig_NoRegister()
{
	return UMaterialInheritanceManagerConfig::GetPrivateStaticClass();
}
struct Z_Construct_UClass_UMaterialInheritanceManagerConfig_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "IncludePath", "MaterialInheritanceManagerConfig.h" },
		{ "ModuleRelativePath", "Public/MaterialInheritanceManagerConfig.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_OutputDirectory_MetaData[] = {
		{ "Category", "Output" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Output directory for auto-created Material Instances (Content Browser path). */" },
#endif
		{ "ModuleRelativePath", "Public/MaterialInheritanceManagerConfig.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Output directory for auto-created Material Instances (Content Browser path)." },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_SearchDepth_MetaData[] = {
		{ "Category", "Display" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Maximum inheritance depth to display in each direction. */" },
#endif
		{ "ModuleRelativePath", "Public/MaterialInheritanceManagerConfig.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Maximum inheritance depth to display in each direction." },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_MaxNodeCount_MetaData[] = {
		{ "Category", "Display" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Maximum number of nodes to display. */" },
#endif
		{ "ModuleRelativePath", "Public/MaterialInheritanceManagerConfig.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Maximum number of nodes to display." },
#endif
	};
#endif // WITH_METADATA

// ********** Begin Class UMaterialInheritanceManagerConfig constinit property declarations ********
	static const UECodeGen_Private::FStrPropertyParams NewProp_OutputDirectory;
	static const UECodeGen_Private::FIntPropertyParams NewProp_SearchDepth;
	static const UECodeGen_Private::FIntPropertyParams NewProp_MaxNodeCount;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Class UMaterialInheritanceManagerConfig constinit property declarations **********
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UMaterialInheritanceManagerConfig>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
}; // struct Z_Construct_UClass_UMaterialInheritanceManagerConfig_Statics

// ********** Begin Class UMaterialInheritanceManagerConfig Property Definitions *******************
const UECodeGen_Private::FStrPropertyParams Z_Construct_UClass_UMaterialInheritanceManagerConfig_Statics::NewProp_OutputDirectory = { "OutputDirectory", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Str, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UMaterialInheritanceManagerConfig, OutputDirectory), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_OutputDirectory_MetaData), NewProp_OutputDirectory_MetaData) };
const UECodeGen_Private::FIntPropertyParams Z_Construct_UClass_UMaterialInheritanceManagerConfig_Statics::NewProp_SearchDepth = { "SearchDepth", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UMaterialInheritanceManagerConfig, SearchDepth), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_SearchDepth_MetaData), NewProp_SearchDepth_MetaData) };
const UECodeGen_Private::FIntPropertyParams Z_Construct_UClass_UMaterialInheritanceManagerConfig_Statics::NewProp_MaxNodeCount = { "MaxNodeCount", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UMaterialInheritanceManagerConfig, MaxNodeCount), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_MaxNodeCount_MetaData), NewProp_MaxNodeCount_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UMaterialInheritanceManagerConfig_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UMaterialInheritanceManagerConfig_Statics::NewProp_OutputDirectory,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UMaterialInheritanceManagerConfig_Statics::NewProp_SearchDepth,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UMaterialInheritanceManagerConfig_Statics::NewProp_MaxNodeCount,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UMaterialInheritanceManagerConfig_Statics::PropPointers) < 2048);
// ********** End Class UMaterialInheritanceManagerConfig Property Definitions *********************
UObject* (*const Z_Construct_UClass_UMaterialInheritanceManagerConfig_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_UObject,
	(UObject* (*)())Z_Construct_UPackage__Script_MaterialInheritanceManager,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UMaterialInheritanceManagerConfig_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_UMaterialInheritanceManagerConfig_Statics::ClassParams = {
	&UMaterialInheritanceManagerConfig::StaticClass,
	"EditorPerProjectUserSettings",
	&StaticCppClassTypeInfo,
	DependentSingletons,
	nullptr,
	Z_Construct_UClass_UMaterialInheritanceManagerConfig_Statics::PropPointers,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	0,
	UE_ARRAY_COUNT(Z_Construct_UClass_UMaterialInheritanceManagerConfig_Statics::PropPointers),
	0,
	0x000000A4u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UMaterialInheritanceManagerConfig_Statics::Class_MetaDataParams), Z_Construct_UClass_UMaterialInheritanceManagerConfig_Statics::Class_MetaDataParams)
};
void UMaterialInheritanceManagerConfig::StaticRegisterNativesUMaterialInheritanceManagerConfig()
{
}
UClass* Z_Construct_UClass_UMaterialInheritanceManagerConfig()
{
	if (!Z_Registration_Info_UClass_UMaterialInheritanceManagerConfig.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UMaterialInheritanceManagerConfig.OuterSingleton, Z_Construct_UClass_UMaterialInheritanceManagerConfig_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_UMaterialInheritanceManagerConfig.OuterSingleton;
}
DEFINE_VTABLE_PTR_HELPER_CTOR_NS(, UMaterialInheritanceManagerConfig);
UMaterialInheritanceManagerConfig::~UMaterialInheritanceManagerConfig() {}
// ********** End Class UMaterialInheritanceManagerConfig ******************************************

// ********** Begin Registration *******************************************************************
struct Z_CompiledInDeferFile_FID_Users_tairuili_Documents_Unreal_Projects_MaterialTest2_Plugins_MaterialInheritanceManager_Source_MaterialInheritanceManager_Public_MaterialInheritanceManagerConfig_h__Script_MaterialInheritanceManager_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_UMaterialInheritanceManagerConfig, UMaterialInheritanceManagerConfig::StaticClass, TEXT("UMaterialInheritanceManagerConfig"), &Z_Registration_Info_UClass_UMaterialInheritanceManagerConfig, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UMaterialInheritanceManagerConfig), 3722823034U) },
	};
}; // Z_CompiledInDeferFile_FID_Users_tairuili_Documents_Unreal_Projects_MaterialTest2_Plugins_MaterialInheritanceManager_Source_MaterialInheritanceManager_Public_MaterialInheritanceManagerConfig_h__Script_MaterialInheritanceManager_Statics 
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_tairuili_Documents_Unreal_Projects_MaterialTest2_Plugins_MaterialInheritanceManager_Source_MaterialInheritanceManager_Public_MaterialInheritanceManagerConfig_h__Script_MaterialInheritanceManager_519234630{
	TEXT("/Script/MaterialInheritanceManager"),
	Z_CompiledInDeferFile_FID_Users_tairuili_Documents_Unreal_Projects_MaterialTest2_Plugins_MaterialInheritanceManager_Source_MaterialInheritanceManager_Public_MaterialInheritanceManagerConfig_h__Script_MaterialInheritanceManager_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_tairuili_Documents_Unreal_Projects_MaterialTest2_Plugins_MaterialInheritanceManager_Source_MaterialInheritanceManager_Public_MaterialInheritanceManagerConfig_h__Script_MaterialInheritanceManager_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0,
};
// ********** End Registration *********************************************************************

PRAGMA_ENABLE_DEPRECATION_WARNINGS
