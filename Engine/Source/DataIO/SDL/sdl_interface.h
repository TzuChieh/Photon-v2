#pragma once

#include "DataIO/SDL/ISdlResource.h"

#include <memory>
#include <type_traits>

namespace ph
{

class SdlClass;

}// end namespace ph

// TODO: static assert the return type of the impl func
#define PH_DEFINE_SDL_CLASS(OwnerType)\
	\
	static_assert(std::is_base_of_v<::ph::ISdlResource, OwnerType>,\
		"Owner type "#OwnerType" must derive from ISdlResource.");\
	\
	static const ::ph::SdlClass& getSdlClass()\
	{\
		using SdlClassType = decltype(sdl_class_impl_#OwnerType());\
		static auto sdlClass = std::make_unique<SdlClassType>(sdl_class_impl_#OwnerType());\
		return *sdlClass;\
	}\
	\
	static decltype(auto) sdl_class_impl_#OwnerType()
