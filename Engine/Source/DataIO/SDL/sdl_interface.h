#pragma once

// Base types
#include "DataIO/SDL/Introspect/SdlClass.h"
#include "DataIO/SDL/Introspect/SdlStruct.h"
#include "DataIO/SDL/Introspect/SdlFunction.h"
#include "DataIO/SDL/Introspect/SdlEnum.h"

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

// Function types
#include "DataIO/SDL/Introspect/TSdlMethod.h"

// Enum types
#include "DataIO/SDL/Introspect/TSdlBasicEnum.h"
#include "DataIO/SDL/Introspect/TSdlEnum.h"

#include <type_traits>

// TODO: need traits helper to verify a sdl class/struct also contains the required macro

#define PH_DEFINE_SDL_CLASS(SDL_CLASS_TYPE)\
	\
	using ClassType = SDL_CLASS_TYPE;\
	using OwnerType = SDL_CLASS_TYPE::OwnerType;\
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

#define PH_DEFINE_SDL_STRUCT(SDL_STRUCT_TYPE)\
	\
	using StructType = SDL_STRUCT_TYPE;\
	using OwnerType  = SDL_STRUCT_TYPE::OwnerType;\
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

#define PH_DEFINE_SDL_FUNCTION(SDL_FUNCTION_TYPE)\
	\
	using FunctionType = SDL_FUNCTION_TYPE;\
	using OwnerType    = SDL_FUNCTION_TYPE::OwnerType;\
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

#define PH_DEFINE_SDL_ENUM(SDL_ENUM_TYPE)\
	template<>\
	class TSdlEnum<ENUM_TYPE> final\
	{\
	private:\
	\
		using SdlEnumType = SDL_ENUM_TYPE;\
		using EnumType    = SDL_ENUM_TYPE::EnumType;\
	\
		static_assert(std::is_enum_v<EnumType>,\
			"EnumType must be an enum. Currently it is not.");\
	\
	public:\
		inline EnumType operator [] (const std::string_view entryName) const\
		{\
			const ::ph::SdlEnum::TEntry<EnumType> entry = getSdlEnum().getTypedEntry(entryName);\
			return entry.value;\
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
	inline static SdlEnumType internal_sdl_enum_impl()\
