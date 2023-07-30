#pragma once

#include "SDL/sdl_fwd.h"
#include "SDL/ESdlTypeCategory.h"
#include "Utility/traits.h"

#include <type_traits>

namespace ph
{

template<typename T>
concept CSdlResource = CDerived<T, ISdlResource>;

/*! @brief Check if SDL category information can be obtained statically.
The result is true if the static member variable `T::CATEGORY` exists, otherwise 
the result is false.
*/
template<typename T>
concept CHasStaticSdlCategoryInfo = 
	CSdlResource<T> &&
	// Check equality of types with cv removed just to be robust
	// (in case the implementer qualifies the `CATEGORY` member).
	std::is_same_v<std::remove_cv_t<decltype(T::CATEGORY)>, ESdlTypeCategory>;

/*! @brief Whether @p T is a well-defined SDL class.
*/
template<typename T>
concept CHasSdlClassDefinition = 
	CSdlResource<T> &&
	std::is_same_v<typename T::SdlClassDefinitionMarker, std::remove_cv_t<T>>;

/*! @brief Whether @p T is a well-defined SDL struct.
*/
template<typename T>
concept CHasSdlStructDefinition =
	std::is_same_v<typename T::SdlStructDefinitionMarker, std::remove_cv_t<T>>;

/*! @brief Whether @p T is a well-defined SDL function.
*/
template<typename T>
concept CHasSdlFunctionDefinition =
	std::is_same_v<typename T::SdlFunctionDefinitionMarker, std::remove_cv_t<T>>;

/*! @brief Whether @p EnumType is a well-defined SDL enum.
*/
template<typename EnumType>
concept CHasSdlEnumDefinition =
	CEnum<EnumType> &&
	std::is_same_v<
		typename TSdlEnum<std::remove_cv_t<EnumType>>::SdlEnumDefinitionMarker,
		std::remove_cv_t<EnumType>>;

template<typename T>
concept CSdlInstance = 
	CSdlResource<T> ||
	CHasSdlStructDefinition<T> ||
	CHasSdlFunctionDefinition<T>;

template<typename T>
concept CSdlStructSupportsInitToDefault = CHasSdlStructDefinition<T> && requires (T instance)
{
	T::getSdlStruct()->initDefaultStruct(instance);
};

}// end namespace ph
