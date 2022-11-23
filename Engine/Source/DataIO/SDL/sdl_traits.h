#pragma once

#include "DataIO/SDL/ETypeCategory.h"

#include <type_traits>

namespace ph
{

class ISdlResource;

template<typename T>
concept CIsSdlResource = std::is_base_of_v<ISdlResource, T>;

/*! @brief Check if SDL category information can be obtained statically.
The result is true if the static member variable T::CATEGORY exists, otherwise the result is false.
*/
template<typename T>
concept CHasStaticSdlCategoryInfo = 
	// Check equality of types with cv and ref removed just to be robust
	// (in case the implementer qualifies the `CATEGORY` member).
	std::is_same_v<std::remove_cvref_t<decltype(T::CATEGORY)>, ETypeCategory>;

template<typename T>
concept CHasSdlClassDefinition = 
	std::is_same_v<typename T::SdlClassDefinitionMarker, void>;

template<typename T>
concept CHasSdlStructDefinition =
	std::is_same_v<typename T::SdlStructDefinitionMarker, void>;

template<typename T>
concept CHasSdlFunctionDefinition =
	std::is_same_v<typename T::SdlFunctionDefinitionMarker, void>;

}// end namespace ph
