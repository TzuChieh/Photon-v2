#pragma once

#include "SDL/ISdlResource.h"
#include "SDL/TSdl.h"
#include "SDL/Introspect/TSdlEnum.h"

// Base types
#include "SDL/Introspect/SdlClass.h"
#include "SDL/Introspect/SdlStruct.h"
#include "SDL/Introspect/SdlFunction.h"
#include "SDL/Introspect/SdlEnum.h"

// Enum types
#include "SDL/Introspect/TSdlGeneralEnum.h"

// Owner types
#include "SDL/Introspect/TSdlOwnerClass.h"
#include "SDL/Introspect/TSdlOwnerStruct.h"

// Field types
#include "SDL/Introspect/TSdlBool.h"
#include "SDL/Introspect/TSdlInteger.h"
#include "SDL/Introspect/TSdlReal.h"
#include "SDL/Introspect/TSdlString.h"
#include "SDL/Introspect/TSdlVector2.h"
#include "SDL/Introspect/TSdlVector3.h"
#include "SDL/Introspect/TSdlQuaternion.h"
#include "SDL/Introspect/TSdlRealArray.h"
#include "SDL/Introspect/TSdlVector3Array.h"
#include "SDL/Introspect/TSdlReference.h"
#include "SDL/Introspect/TSdlReferenceArray.h"
#include "SDL/Introspect/TSdlEnumField.h"
#include "SDL/Introspect/TSdlPath.h"
#include "SDL/Introspect/TSdlResourceIdentifier.h"

// Function types
#include "SDL/Introspect/TSdlMethod.h"

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
  - A static method for accessing the static SDL class.

* `const SdlClass* getDynamicSdlClass() const`
  - A virtual method for accessing SDL class in runtime, through an instance to resource. The returned
    class will be the one defined for the actual type of the resource.

*/
#define PH_DEFINE_SDL_CLASS(...)/* variadic args for template types that contain commas */\
	\
	using ClassType = std::remove_cv_t<__VA_ARGS__>;\
	using OwnerType = std::remove_cv_t<typename ClassType::OwnerType>;\
	\
	/* A marker so we know the macro has been called. */\
	using SdlClassDefinitionMarker = OwnerType;\
	\
	inline static const ClassType* getSdlClass()\
	{\
		static_assert(std::is_base_of_v<::ph::ISdlResource, OwnerType>,\
			"PH_DEFINE_SDL_CLASS() can only be defined for SDL resource.");\
		static_assert(std::is_base_of_v<::ph::SdlClass, ClassType>,\
			"PH_DEFINE_SDL_CLASS() must return a class derived from SdlClass.");\
		\
		static const ClassType sdlClass = internal_sdl_class_impl();\
		return &sdlClass;\
	}\
	\
	inline const ::ph::SdlClass* getDynamicSdlClass() const override\
	{\
		return getSdlClass();\
	}\
	\
	inline static ClassType internal_sdl_class_impl()

#define PH_DEFINE_SDL_STRUCT(...)/* variadic args for template types that contain commas */\
	\
	using StructType = std::remove_cv_t<__VA_ARGS__>;\
	using OwnerType  = std::remove_cv_t<typename StructType::OwnerType>;\
	\
	/* A marker so we know the macro has been called. */\
	using SdlStructDefinitionMarker = OwnerType;\
	\
	inline static const StructType* getSdlStruct()\
	{\
		static_assert(std::is_base_of_v<::ph::SdlStruct, StructType>,\
			"PH_DEFINE_SDL_STRUCT() must return a struct derived from SdlStruct.");\
		\
		static const StructType sdlStruct = internal_sdl_struct_impl();\
		return &sdlStruct;\
	}\
	\
	inline static StructType internal_sdl_struct_impl()

#define PH_DEFINE_SDL_FUNCTION(...)/* variadic args for template types that contain commas */\
	\
	using FunctionType = std::remove_cv_t<__VA_ARGS__>;\
	using OwnerType    = std::remove_cv_t<typename FunctionType::OwnerType>;\
	\
	/* A marker so we know the macro has been called. */\
	using SdlFunctionDefinitionMarker = OwnerType;\
	\
	inline static const FunctionType* getSdlFunction()\
	{\
		static_assert(std::is_base_of_v<::ph::SdlFunction, FunctionType>,\
			"PH_DEFINE_SDL_FUNCTION() must return a function derived from SdlFunction.");\
		\
		static const FunctionType sdlFunction = internal_sdl_function_impl();\
		return &sdlFunction;\
	}\
	\
	inline static FunctionType internal_sdl_function_impl()

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
#define PH_DEFINE_SDL_ENUM(...)/* variadic args for template types that contain commas */\
	template<>\
	class TSdlEnum<std::remove_cv_t<typename __VA_ARGS__::EnumType>> final\
	{\
	public:\
	\
		using SdlEnumType = std::remove_cv_t<__VA_ARGS__>;\
		using EnumType    = std::remove_cv_t<typename SdlEnumType::EnumType>;\
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
		inline static const SdlEnumType* getSdlEnum()\
		{\
			static_assert(std::is_base_of_v<::ph::SdlEnum, SdlEnumType>,\
				"PH_DEFINE_SDL_ENUM() must return an enum derived from SdlEnum.");\
			\
			static const SdlEnumType sdlEnum = internal_sdl_enum_impl();\
			return &sdlEnum;\
		}\
	\
	private:\
		static SdlEnumType internal_sdl_enum_impl();\
	};\
	\
	/* In-header Implementations: */\
	\
	inline __VA_ARGS__ TSdlEnum<std::remove_cv_t<typename __VA_ARGS__::EnumType>>::internal_sdl_enum_impl()
