#pragma once

// Base types
#include "DataIO/SDL/Introspect/SdlClass.h"
#include "DataIO/SDL/Introspect/SdlStruct.h"
#include "DataIO/SDL/Introspect/SdlFunction.h"

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

#include <type_traits>

// TODO: need traits helper to verify a sdl class/struct also contains the required macro

#define PH_DEFINE_SDL_CLASS(SDL_CLASS_TYPE)\
	\
	using ClassType = SDL_CLASS_TYPE;\
	using OwnerType = SDL_CLASS_TYPE::OwnerType;\
	\
	inline static const ClassType& getSdlClass()\
	{\
		static_assert(std::is_base_of_v<::ph::SdlClass, ClassType>,\
			"PH_DEFINE_SDL_CLASS() must return a class derived from SdlClass.");\
		\
		static const ClassType sdlClass = internal_sdl_class_impl();\
		return sdlClass;\
	}\
	\
	inline static ClassType internal_sdl_class_impl()

#define PH_DEFINE_SDL_STRUCT(SDL_STRUCT_TYPE)\
	\
	using StructType = SDL_STRUCT_TYPE;\
	using OwnerType  = SDL_STRUCT_TYPE::OwnerType;\
	\
	inline static const StructType& getSdlStruct()\
	{\
		static_assert(std::is_base_of_v<::ph::SdlStruct, StructType>,\
			"PH_DEFINE_SDL_STRUCT() must return a struct derived from SdlStruct.");\
		\
		static const StructType sdlStruct = internal_sdl_struct_impl();\
		return sdlStruct;\
	}\
	\
	inline static StructType internal_sdl_struct_impl()

#define PH_DEFINE_SDL_FUNCTION()\
	inline static decltype(auto) getSdlFunction()\
	{\
		using SdlFunctionType = decltype(sdl_function_impl_internal());\
		static_assert(std::is_base_of_v<::ph::SdlFunction, SdlFunctionType>,\
			"PH_DEFINE_SDL_FUNCTION() must return a function derived from SdlFunction.");\
		\
		static const auto sdlFunction = internal_sdl_function_impl();\
		return sdlFunction;\
	}\
	\
	inline static decltype(auto) internal_sdl_function_impl()
