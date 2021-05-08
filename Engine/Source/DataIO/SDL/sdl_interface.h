#pragma once

#include "DataIO/SDL/Introspect/SdlClass.h"
#include "DataIO/SDL/Introspect/SdlStruct.h"

#include <type_traits>

namespace ph
{

class SdlClass;

}// end namespace ph

#define PH_DEFINE_SDL_CLASS(OwnerType)\
	inline static decltype(auto) getSdlClass()\
	{\
		using SdlClassType = decltype(sdl_class_impl_##OwnerType());\
		static_assert(std::is_base_of_v<::ph::SdlClass, SdlClassType>,\
			"PH_DEFINE_SDL_CLASS() must return a class derived from SdlClass.");\
		\
		static const auto sdlClass = sdl_class_impl_##OwnerType();\
		return sdlClass;\
	}\
	\
	inline static decltype(auto) sdl_class_impl_##OwnerType()

#define PH_DEFINE_SDL_STRUCT(StructType)\
	inline static decltype(auto) getSdlStruct()\
	{\
		using SdlStructType = decltype(sdl_class_impl_##StructType());\
		static_assert(std::is_base_of_v<::ph::SdlStruct, SdlStructType>,\
			"PH_DEFINE_SDL_STRUCT() must return a struct derived from SdlStruct.");\
		\
		static const auto sdlStruct = sdl_struct_impl_##StructType();\
		return sdlStruct;\
	}\
	\
	inline static decltype(auto) sdl_struct_impl_##StructType()
