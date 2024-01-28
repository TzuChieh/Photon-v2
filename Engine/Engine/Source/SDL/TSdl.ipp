#pragma once

#include "SDL/TSdl.h"
#include "SDL/ISdlResource.h"
#include "SDL/Introspect/SdlClass.h"
#include "SDL/Introspect/SdlStruct.h"
#include "SDL/sdl_helpers.h"

#include <Common/assertion.h>
#include <Common/logging.h>

#include <utility>

namespace ph
{

inline std::shared_ptr<ISdlResource> TSdl<void>::makeResource(const SdlClass* clazz)
{
	if(!clazz)
	{
		return nullptr;
	}

	// Creates an uninitialized resource
	std::shared_ptr<ISdlResource> resource = clazz->createResource();

	// Could be empty due to `T` being abstract or being defined to be uninstantiable
	if(!resource)
	{
		PH_DEFAULT_LOG(Warning,
			"default resource creation failed, {} could be abstract or defined to be uninstantiable",
			clazz->genPrettyName());
		return nullptr;
	}

	clazz->initDefaultResource(*resource);
	return resource;
}

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

	std::shared_ptr<ISdlResource> resource = TSdl<>::makeResource(clazz);
	if(!resource)
	{
		return nullptr;
	}

	// Obtain typed resource. This dynamic cast also guard against the case where
	// `T` might not actually have SDL class defined locally but inherited (i.e., the resource
	// created was in fact not of type `T`).
	std::shared_ptr<T> typedResource = std::dynamic_pointer_cast<T>(std::move(resource));
	if(!typedResource)
	{
		PH_DEFAULT_LOG(Warning,
			"default resource creation failed, {} may not have SDL class defined",
			clazz->genPrettyName());
	}
	return typedResource;
}

template<CSdlResource T>
template<typename... DeducedArgs>
inline T TSdl<T>::make(DeducedArgs&&... args)
{
	static_assert(std::is_constructible_v<T, DeducedArgs...>,
		"SDL class type T is not constructible using the specified arguments.");

	T instance(std::forward<DeducedArgs>(args)...);
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

template<CSdlResource T>
inline std::shared_ptr<T> TSdl<T>::loadResource(const Path& file)
{
	if constexpr(CHasSdlClassDefinition<T>)
	{
		auto loadedResource = detail::load_single_resource(T::getSdlClass(), file);
#if PH_DEBUG
		if(loadedResource)
		{
			PH_ASSERT(std::dynamic_pointer_cast<T>(loadedResource));
		}
#endif
		
		// Static cast is enough here as the result is guaranteed to have the specified
		// class if non-null.
		return std::static_pointer_cast<T>(loadedResource);
	}
	else
	{
		PH_STATIC_ASSERT_DEPENDENT_FALSE(T,
			"Cannot load resource without SDL class definition. Did you call PH_DEFINE_SDL_CLASS() "
			"in the body of type T?");
	}
}

template<CSdlResource T>
inline void TSdl<T>::saveResource(const std::shared_ptr<T>& resource, const Path& file)
{
	detail::save_single_resource(resource, file);
}

}// end namespace ph
