#pragma once

#include "DataIO/SDL/Introspect/SdlClass.h"
#include "DataIO/SDL/Introspect/SdlStruct.h"
#include "DataIO/SDL/Introspect/SdlFunction.h"

#include <type_traits>

// TODO: need traits helper to verify a sdl class/struct also contains the required macro

#define PH_DEFINE_SDL_CLASS()\
	inline static decltype(auto) getSdlClass()\
	{\
		using SdlClassType = decltype(sdl_class_impl_internal());\
		static_assert(std::is_base_of_v<::ph::SdlClass, SdlClassType>,\
			"PH_DEFINE_SDL_CLASS() must return a class derived from SdlClass.");\
		\
		static const auto sdlClass = sdl_class_impl_internal();\
		return sdlClass;\
	}\
	\
	inline static decltype(auto) sdl_class_impl_internal()

#define PH_DEFINE_SDL_STRUCT()\
	inline static decltype(auto) getSdlStruct()\
	{\
		using SdlStructType = decltype(sdl_struct_impl_internal());\
		static_assert(std::is_base_of_v<::ph::SdlStruct, SdlStructType>,\
			"PH_DEFINE_SDL_STRUCT() must return a struct derived from SdlStruct.");\
		\
		static const auto sdlStruct = sdl_struct_impl_internal();\
		return sdlStruct;\
	}\
	\
	inline static decltype(auto) sdl_class_impl_internal()

#define PH_DEFINE_SDL_FUNCTION()\
	inline static decltype(auto) getSdlFunction()\
	{\
		using SdlFunctionType = decltype(sdl_function_impl_internal());\
		static_assert(std::is_base_of_v<::ph::SdlFunction, SdlFunctionType>,\
			"PH_DEFINE_SDL_FUNCTION() must return a function derived from SdlFunction.");\
		\
		static const auto sdlFunction = sdl_function_impl_internal();\
		return sdlFunction;\
	}\
	\
	inline static decltype(auto) sdl_function_impl_internal()
