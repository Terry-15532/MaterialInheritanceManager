// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "MaterialInheritanceGraphSchema.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
static_assert(!UE_WITH_CONSTINIT_UOBJECT, "This generated code can only be compiled with !UE_WITH_CONSTINIT_OBJECT");
void EmptyLinkFunctionForGeneratedCodeMaterialInheritanceGraphSchema() {}

// ********** Begin Cross Module References ********************************************************
ENGINE_API UClass* Z_Construct_UClass_UEdGraphSchema();
MATERIALINHERITANCEMANAGER_API UClass* Z_Construct_UClass_UMaterialInheritanceGraphSchema();
MATERIALINHERITANCEMANAGER_API UClass* Z_Construct_UClass_UMaterialInheritanceGraphSchema_NoRegister();
UPackage* Z_Construct_UPackage__Script_MaterialInheritanceManager();
// ********** End Cross Module References **********************************************************

// ********** Begin Class UMaterialInheritanceGraphSchema ******************************************
FClassRegistrationInfo Z_Registration_Info_UClass_UMaterialInheritanceGraphSchema;
UClass* UMaterialInheritanceGraphSchema::GetPrivateStaticClass()
{
	using TClass = UMaterialInheritanceGraphSchema;
	if (!Z_Registration_Info_UClass_UMaterialInheritanceGraphSchema.InnerSingleton)
	{
		GetPrivateStaticClassBody(
			TClass::StaticPackage(),
			TEXT("MaterialInheritanceGraphSchema"),
			Z_Registration_Info_UClass_UMaterialInheritanceGraphSchema.InnerSingleton,
			StaticRegisterNativesUMaterialInheritanceGraphSchema,
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
	return Z_Registration_Info_UClass_UMaterialInheritanceGraphSchema.InnerSingleton;
}
UClass* Z_Construct_UClass_UMaterialInheritanceGraphSchema_NoRegister()
{
	return UMaterialInheritanceGraphSchema::GetPrivateStaticClass();
}
struct Z_Construct_UClass_UMaterialInheritanceGraphSchema_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "IncludePath", "MaterialInheritanceGraphSchema.h" },
		{ "ModuleRelativePath", "Public/MaterialInheritanceGraphSchema.h" },
	};
#endif // WITH_METADATA

// ********** Begin Class UMaterialInheritanceGraphSchema constinit property declarations **********
// ********** End Class UMaterialInheritanceGraphSchema constinit property declarations ************
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UMaterialInheritanceGraphSchema>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
}; // struct Z_Construct_UClass_UMaterialInheritanceGraphSchema_Statics
UObject* (*const Z_Construct_UClass_UMaterialInheritanceGraphSchema_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_UEdGraphSchema,
	(UObject* (*)())Z_Construct_UPackage__Script_MaterialInheritanceManager,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UMaterialInheritanceGraphSchema_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_UMaterialInheritanceGraphSchema_Statics::ClassParams = {
	&UMaterialInheritanceGraphSchema::StaticClass,
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
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UMaterialInheritanceGraphSchema_Statics::Class_MetaDataParams), Z_Construct_UClass_UMaterialInheritanceGraphSchema_Statics::Class_MetaDataParams)
};
void UMaterialInheritanceGraphSchema::StaticRegisterNativesUMaterialInheritanceGraphSchema()
{
}
UClass* Z_Construct_UClass_UMaterialInheritanceGraphSchema()
{
	if (!Z_Registration_Info_UClass_UMaterialInheritanceGraphSchema.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UMaterialInheritanceGraphSchema.OuterSingleton, Z_Construct_UClass_UMaterialInheritanceGraphSchema_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_UMaterialInheritanceGraphSchema.OuterSingleton;
}
UMaterialInheritanceGraphSchema::UMaterialInheritanceGraphSchema(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {}
DEFINE_VTABLE_PTR_HELPER_CTOR_NS(, UMaterialInheritanceGraphSchema);
UMaterialInheritanceGraphSchema::~UMaterialInheritanceGraphSchema() {}
// ********** End Class UMaterialInheritanceGraphSchema ********************************************

// ********** Begin Registration *******************************************************************
struct Z_CompiledInDeferFile_FID_Users_tairuili_Documents_Unreal_Projects_MaterialTest2_Plugins_MaterialInheritanceManager_Source_MaterialInheritanceManager_Public_MaterialInheritanceGraphSchema_h__Script_MaterialInheritanceManager_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_UMaterialInheritanceGraphSchema, UMaterialInheritanceGraphSchema::StaticClass, TEXT("UMaterialInheritanceGraphSchema"), &Z_Registration_Info_UClass_UMaterialInheritanceGraphSchema, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UMaterialInheritanceGraphSchema), 1005686525U) },
	};
}; // Z_CompiledInDeferFile_FID_Users_tairuili_Documents_Unreal_Projects_MaterialTest2_Plugins_MaterialInheritanceManager_Source_MaterialInheritanceManager_Public_MaterialInheritanceGraphSchema_h__Script_MaterialInheritanceManager_Statics 
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_tairuili_Documents_Unreal_Projects_MaterialTest2_Plugins_MaterialInheritanceManager_Source_MaterialInheritanceManager_Public_MaterialInheritanceGraphSchema_h__Script_MaterialInheritanceManager_1168415290{
	TEXT("/Script/MaterialInheritanceManager"),
	Z_CompiledInDeferFile_FID_Users_tairuili_Documents_Unreal_Projects_MaterialTest2_Plugins_MaterialInheritanceManager_Source_MaterialInheritanceManager_Public_MaterialInheritanceGraphSchema_h__Script_MaterialInheritanceManager_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_tairuili_Documents_Unreal_Projects_MaterialTest2_Plugins_MaterialInheritanceManager_Source_MaterialInheritanceManager_Public_MaterialInheritanceGraphSchema_h__Script_MaterialInheritanceManager_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0,
};
// ********** End Registration *********************************************************************

PRAGMA_ENABLE_DEPRECATION_WARNINGS
