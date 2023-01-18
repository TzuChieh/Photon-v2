#include "DataIO/SDL/TSdl.h"
#include "DataIO/SDL/Introspect/SdlClass.h"
#include "DataIO/SDL/Introspect/SdlStruct.h"
#include "DataIO/SDL/sdl_helpers.h"
#include "Common/assertion.h"

#include "Common/logging.h"

#include <memory>

namespace ph
{

template<CIsSdlResource T>
inline constexpr ETypeCategory TSdl<T>::getCategory()
{
	return sdl::category_of<T>();
}

template<CIsSdlResource T>
inline std::shared_ptr<T> TSdl<T>::makeResource()
{
	static_assert(CHasSdlClassDefinition<T>,
		"No SDL class definition found. Did you call PH_DEFINE_SDL_CLASS() in the body of type T?");

	const SdlClass* clazz = T::getSdlClass();
	PH_ASSERT(clazz);

	// Creates an uninitialized resource
	std::shared_ptr<ISdlResource> resource = clazz->createResource();

	// Could be empty due to `T` being abstract or being defined to be
	if(!resource)
	{
		return nullptr;
	}

	clazz->initDefaultResource(*resource);

	// This dynamic cast is required in the sense that `T` might not actually have SDL class
	// defined locally but inherited; the cast guard against this case.
	std::shared_ptr<T> typedResource = std::dynamic_pointer_cast<T>(std::move(resource));
	if(!typedResource)
	{
		PH_DEFAULT_LOG_WARNING(
			"default resource creation failed, the type specified may not have SDL class defined");
	}
	return typedResource;
}

template<CIsSdlResource T>
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
