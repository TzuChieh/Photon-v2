#pragma once

#include "Engine/SDL/sdl_fwd.h"
#include "Engine/SDL/ISdlResource.h"
#include "Engine/SDL/TSdl.h"
#include "Engine/SDL/Introspect/TSdlEnum.h"
#include "Engine/Utility/traits.h"

// Definer types
#include "Engine/SDL/Definition/TSdlClassDefiner.h"
#include "Engine/SDL/Definition/TSdlStructDefiner.h"
#include "Engine/SDL/Definition/TSdlFunctionDefiner.h"
#include "Engine/SDL/Definition/TSdlEnumDefiner.h"

// Base types
#include "Engine/SDL/Introspect/SdlClass.h"
#include "Engine/SDL/Introspect/SdlStruct.h"
#include "Engine/SDL/Introspect/SdlFunction.h"
#include "Engine/SDL/Introspect/SdlEnum.h"

// Owner types
#include "Engine/SDL/Introspect/TSdlOwnerClass.h"
#include "Engine/SDL/Introspect/TSdlOwnerStruct.h"
#include "Engine/SDL/Introspect/TSdlOwnerMethod.h"

// Enum types
#include "Engine/SDL/Introspect/TSdlGeneralEnum.h"

// Field types
#include "Engine/SDL/Introspect/TSdlBool.h"
#include "Engine/SDL/Introspect/TSdlInteger.h"
#include "Engine/SDL/Introspect/TSdlReal.h"
#include "Engine/SDL/Introspect/TSdlString.h"
#include "Engine/SDL/Introspect/TSdlVector2.h"
#include "Engine/SDL/Introspect/TSdlVector3.h"
#include "Engine/SDL/Introspect/TSdlVector4.h"
#include "Engine/SDL/Introspect/TSdlQuaternion.h"
#include "Engine/SDL/Introspect/TSdlRealArray.h"
#include "Engine/SDL/Introspect/TSdlVector3Array.h"
#include "Engine/SDL/Introspect/TSdlReference.h"
#include "Engine/SDL/Introspect/TSdlReferenceArray.h"
#include "Engine/SDL/Introspect/TSdlEnumField.h"
#include "Engine/SDL/Introspect/TSdlPath.h"
#include "Engine/SDL/Introspect/TSdlResourceIdentifier.h"
#include "Engine/SDL/Introspect/TSdlStructArray.h"

#include <type_traits>

/*! @brief Define a SDL class with function-like syntax.

Use this macro in the class body of a SDL resource to provide an interface
for introspection. The SDL class instance type to be defined should be the only
argument to the macro. Followed is a ordinary C++ scope (a pair of curly braces)
where the definition for the SDL class instance resides. Several utility types 
can be used within the scope: `ClassType` for the type of the SDL class, and
`OwnerType` for the type of the SDL resource. Finally, use a return statement 
to return the SDL class instance.

Available functionalities after defining the macro:

* `const ClassType* getSdlClass()`
  - A static method for accessing the SDL class static type.

* `const SdlClass* getDynamicSdlClass() const`
  - A virtual method for accessing SDL class in runtime, through an instance to resource. The returned
    class will be the one defined for the actual type of the resource.

*/
#define PH_DEFINE_SDL_CLASS(ownerType, classDef, ...)\
	\
	using OwnerType = ownerType;\
	\
	/* A marker so we know the macro has been called. */\
	using SdlClassDefinitionMarker = OwnerType;\
	\
	static auto getSdlClass()\
	-> const TSdlOwnerClass<OwnerType>*;\
	\
	inline const ::ph::SdlClass* getDynamicSdlClass() const override\
	{\
		return getSdlClass();\
	}\
	\
	template<typename InternalDef>\
	inline static void internal_sdlClassDefinition(TSdlClassDefiner<InternalDef>& classDef)

/*! @brief Define a SDL struct with function-like syntax.

Available functionalities after defining the macro:

* `const StructType* getSdlStruct()`
  - A static method for accessing the SDL struct static type.

*/
#define PH_DEFINE_SDL_STRUCT(ownerType, structDef, ...)\
	\
	using OwnerType  = ownerType;\
	\
	/* A marker so we know the macro has been called. */\
	using SdlStructDefinitionMarker = OwnerType;\
	\
	static auto getSdlStruct()\
	-> const TSdlOwnerStruct<OwnerType>*;\
	\
	template<typename InternalDef>\
	inline static void internal_sdlStructDefinition(TSdlStructDefiner<InternalDef>& structDef)

/*! @brief Define a SDL method with function-like syntax.

Available functionalities after defining the macro:

* `const FunctionType* getSdlFunction()`
  - A static method for accessing the SDL method static type.

*/
#define PH_DEFINE_SDL_METHOD(ownerType, funcDef, ...)\
	\
	using OwnerType = ownerType;\
	\
	/* A marker so we know the macro has been called. */\
	using SdlFunctionDefinitionMarker = OwnerType;\
	\
	static auto getSdlFunction()\
	-> const TSdlOwnerMethod<OwnerType, std::remove_cvref_t<TCallableTraits<OwnerType>::ArgTypeAt<0>>>*;\
	\
	template<typename InternalDef>\
	inline static void internal_sdlFunctionDefinition(TSdlFunctionDefiner<InternalDef>& funcDef)

/*! @brief Define a SDL enum with function-like syntax.

One does not need to define all enum entries. It is valid to partially expose some enum entries
and keep the others hidden from SDL. Note that it is highly encouraged to provide a valid entry
with the value 0 as it will be used as the default.

A limitation of this macro is that it must be called in the enclosing namespace of `ph` (within
namespace `ph` itself). Note this should not cause any name collision--the enum type can still be 
defined in an appropriate namespace. (The underlying implementation depends on explicit template
specialization which requires the aforementioned rule. As long as a unique enum type is used for
specializing `TSdlEnum`, the resulting type should still be unique.)

For a enum `EnumType`, after the definition is done by calling this macro, you can use methods in
`TSdlEnum<EnumType>` to access functionalities. See `TSdlEnum` for what methods are available. 
*/
#define PH_DEFINE_SDL_ENUM(enumType, enumDef, ...)\
	template<>\
	class TSdlEnum<std::remove_cv_t<typename enumType>> final\
	{\
	public:\
	\
		using SdlEnumType = std::remove_cv_t<::ph::TSdlGeneralEnum<typename enumType>>;\
		using EnumType    = std::remove_cv_t<typename enumType>;\
	\
		static_assert(std::is_enum_v<EnumType>,\
			"EnumType must be an enum. Currently it is not.");\
	\
		/* A marker so we know the macro has been called. */\
		using SdlEnumDefinitionMarker = EnumType;\
	\
	public:\
		inline EnumType operator [] (const std::string_view entryName) const\
		{\
			const ::ph::SdlEnum::TEntry<EnumType> entry = getSdlEnum()->getTypedEntry(entryName);\
			return entry.value;\
		}\
	\
		inline std::string_view operator [] (const EnumType entryValue) const\
		{\
			const ::ph::SdlEnum::TEntry<EnumType> entry = getSdlEnum()->getTypedEntry(entryValue);\
			return entry.name;\
		}\
	\
		static auto getSdlEnum()\
		-> const SdlEnumType*;\
	\
	private:\
		template<typename InternalDef>\
		static void internal_sdlEnumDefinition(TSdlEnumDefiner<InternalDef>& enumDef);\
	};\
	\
	/* In-header Implementations: */\
	\
	template<typename InternalDef>\
	inline void TSdlEnum<std::remove_cv_t<typename enumType>>::internal_sdlEnumDefinition(TSdlEnumDefiner<InternalDef>& enumDef)
