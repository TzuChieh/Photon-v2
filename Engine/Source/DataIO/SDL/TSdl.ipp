#pragma once

#include "DataIO/SDL/TSdl.h"
#include "DataIO/SDL/Introspect/SdlClass.h"
#include "DataIO/SDL/Introspect/SdlStruct.h"
#include "DataIO/SDL/sdl_helpers.h"
#include "Common/assertion.h"

#include "Common/logging.h"

#include <memory>

namespace ph
{

template<CSdlResource T>
inline constexpr ESdlTypeCategory TSdl<T>::getCategory()
{
	return sdl::category_of<T>();
}

template<CSdlResource T>
inline std::shared_ptr<T> TSdl<T>::makeResource()
{
	static_assert(CHasSdlClassDefinition<T>,
		"No SDL class definition found. Did you call PH_DEFINE_SDL_CLASS() in the body of type T?");

	const SdlClass* clazz = T::getSdlClass();
	PH_ASSERT(clazz);

	// Creates an uninitialized resource
	std::shared_ptr<ISdlResource> resource = clazz->createResource();

	// Could be empty due to `T` being abstract or being defined to be uninstantiable
	if(!resource)
	{
		PH_DEFAULT_LOG_WARNING(
			"default resource creation failed, {} could be abstract or defined to be uninstantiable",
			clazz->genPrettyName());

		return nullptr;
	}

	clazz->initDefaultResource(*resource);

	// Obtain typed resource. This dynamic cast also guard against the case where
	// `T` might not actually have SDL class defined locally but inherited.
	std::shared_ptr<T> typedResource = std::dynamic_pointer_cast<T>(std::move(resource));
	if(!typedResource)
	{
		PH_DEFAULT_LOG_WARNING(
			"default resource creation failed, {} may not have SDL class defined",
			clazz->genPrettyName());
	}
	return typedResource;
}

template<CSdlResource T>
inline T TSdl<T>::make()
{
	static_assert(std::is_default_constructible_v<T>,
		"T must be default constructible");

	T instance;
	if constexpr(CHasSdlClassDefinition<T>)
	{
		const SdlClass* clazz = T::getSdlClass();
		PH_ASSERT(clazz);

		clazz->initDefaultResource(instance);
	}
	else if(CHasSdlStructDefinition<T>)
	{
		static_assert(CSdlStructSupportsInitToDefault<T>,
			"SDL struct definition of T does not support initializing to default values");

		const auto* ztruct = T::getSdlStruct();
		PH_ASSERT(ztruct);

		ztruct->initDefaultStruct(instance);
	}
	else
	{
		static_assert(CHasSdlClassDefinition<T> || CHasSdlStructDefinition<T>,
			"No SDL class/struct definition found. Did you call "
			"PH_DEFINE_SDL_CLASS()/PH_DEFINE_SDL_STRUCT() in the body of type T?");
	}

	return instance;
}

}// end namespace ph
