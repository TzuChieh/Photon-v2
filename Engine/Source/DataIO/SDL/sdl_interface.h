#pragma once

// Base types
#include "DataIO/SDL/Introspect/SdlClass.h"
#include "DataIO/SDL/Introspect/SdlStruct.h"
#include "DataIO/SDL/Introspect/SdlFunction.h"
#include "DataIO/SDL/Introspect/SdlEnum.h"

// Enum types
#include "DataIO/SDL/Introspect/TBasicSdlEnum.h"
#include "DataIO/SDL/Introspect/TSdlEnum.h"

// Owner types
#include "DataIO/SDL/Introspect/TOwnerSdlClass.h"
#include "DataIO/SDL/Introspect/TOwnerSdlStruct.h"

// Field types
#include "DataIO/SDL/Introspect/TSdlInteger.h"
#include "DataIO/SDL/Introspect/TSdlReal.h"
#include "DataIO/SDL/Introspect/TSdlString.h"
#include "DataIO/SDL/Introspect/TSdlVector3.h"
#include "DataIO/SDL/Introspect/TSdlQuaternion.h"
#include "DataIO/SDL/Introspect/TSdlRealArray.h"
#include "DataIO/SDL/Introspect/TSdlVector3Array.h"
#include "DataIO/SDL/Introspect/TSdlReference.h"
#include "DataIO/SDL/Introspect/TSdlEnumField.h"

// Function types
#include "DataIO/SDL/Introspect/TSdlMethod.h"

#include <type_traits>

// TODO: need traits helper to verify a sdl class/struct also contains the required macro

/*! @brief Define a SDL class with function-like syntax.

Use this macro in the class body of a SDL resource to provide an interface
for introspection. The SDL class instance type to be defined should be the only
argument to the macro. Followed is a ordinary C++ scope (a pair of curly braces)
where the definition for the SDL class instance resides. Several utility types 
can be used within the scope: `ClassType` for the type of the SDL class, and
`OwnerType` for the type of the SDL resource. Finally, use a return statement 
to return the SDL class instance. After defining the SDL class instance using
this macro, the static method `const ClassType* getSdlClass()` will be available
for the SDL resource to access the SDL class instance.
*/
#define PH_DEFINE_SDL_CLASS(...)\
	\
	using ClassType = decltype(__VA_ARGS__);\
	using OwnerType = ClassType::OwnerType;\
	\
	inline static const ClassType* getSdlClass()\
	{\
		static_assert(std::is_base_of_v<::ph::SdlClass, ClassType>,\
			"PH_DEFINE_SDL_CLASS() must return a class derived from SdlClass.");\
		\
		static const ClassType sdlClass = internal_sdl_class_impl();\
		return &sdlClass;\
	}\
	\
	inline static ClassType internal_sdl_class_impl()

#define PH_DEFINE_SDL_STRUCT(...)\
	\
	using StructType = decltype(__VA_ARGS__);\
	using OwnerType  = StructType::OwnerType;\
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

#define PH_DEFINE_SDL_FUNCTION(...)\
	\
	using FunctionType = decltype(__VA_ARGS__);\
	using OwnerType    = FunctionType::OwnerType;\
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

#define PH_DEFINE_SDL_ENUM(...)\
	template<>\
	class TSdlEnum<decltype(__VA_ARGS__)::EnumType> final\
	{\
	private:\
	\
		using SdlEnumType = decltype(__VA_ARGS__);\
		using EnumType    = typename SdlEnumType::EnumType;\
	\
		static_assert(std::is_enum_v<EnumType>,\
			"EnumType must be an enum. Currently it is not.");\
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
				"PH_DEFINE_SDL_ENUM() must return a enum derived from SdlEnum.");\
			\
			static const SdlEnumType sdlEnum = internal_sdl_enum_impl();\
			return &sdlEnum;\
		}\
	\
	private:\
		static SdlEnumType internal_sdl_enum_impl();\
	};\
	\
	\// In-header Implementations:
	\
	inline decltype(__VA_ARGS__) TSdlEnum<typename decltype(__VA_ARGS__)::EnumType>::internal_sdl_enum_impl()
