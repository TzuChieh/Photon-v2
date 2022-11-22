#include "DataIO/SDL/TSdl.h"
#include "DataIO/SDL/Introspect/SdlClass.h"
#include "DataIO/SDL/sdl_helpers.h"
#include "Common/assertion.h"

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
	std::shared_ptr<T> resource = clazz->createResource();

	// Could be empty due to `T` being abstract or being defined to be
	if(!resource)
	{
		return nullptr;
	}

	clazz->initDefaultResource(*resource);
	return resource;
}

}// end namespace ph
