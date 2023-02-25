#pragma once

#include "DataIO/SDL/ETypeCategory.h"

#include <type_traits>

namespace ph
{

class ISdlResource;

template<typename T>
concept CSdlResource = std::is_base_of_v<ISdlResource, T>;

/*! @brief Check if SDL category information can be obtained statically.
The result is true if the static member variable `T::CATEGORY` exists, otherwise 
the result is false.
*/
template<typename T>
concept CHasStaticSdlCategoryInfo = 
	CSdlResource<T> &&
	// Check equality of types with cv and ref removed just to be robust
	// (in case the implementer qualifies the `CATEGORY` member).
	std::is_same_v<std::remove_cvref_t<decltype(T::CATEGORY)>, ETypeCategory>;

/*! @brief Whether @p T is a well-defined SDL class.
*/
template<typename T>
concept CHasSdlClassDefinition = 
	CSdlResource<T> &&
	std::is_same_v<typename T::SdlClassDefinitionMarker, T>;

/*! @brief Whether @p T is a well-defined SDL struct.
*/
template<typename T>
concept CHasSdlStructDefinition =
	std::is_same_v<typename T::SdlStructDefinitionMarker, T>;

/*! @brief Whether @p T is a well-defined SDL function.
*/
template<typename T>
concept CHasSdlFunctionDefinition =
	std::is_same_v<typename T::SdlFunctionDefinitionMarker, T>;

template<typename T>
concept CSdlStructSupportsInitToDefault = CHasSdlStructDefinition<T> && requires (T instance)
{
	T::getSdlStruct()->initDefaultStruct(instance);
};

}// end namespace ph
