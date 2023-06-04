#pragma once

/*! @file

@brief SDL resource helpers.
*/

#include "SDL/ISdlResource.h"
#include "SDL/ESdlTypeCategory.h"
#include "SDL/sdl_traits.h"

#include <type_traits>
#include <concepts>
#include <memory>

namespace ph
{

template<CSdlResource T>
class TSdl final
{
public:
	/*! @brief Statically gets the category of @p T.
	@tparam T Type that category information is going to be extracted from.
	@return Category of @p T. The result is `ESdlTypeCategory::Unspecified` if category
	information does not exist.
	*/
	static constexpr ESdlTypeCategory getCategory();

	/*! @brief Creates a sharable resource filled with default values.
	Default values are determined by SDL class definition.
	*/
	static std::shared_ptr<T> makeResource();

	/*! @brief Creates a resource or struct instance filled with default values.
	Default values are determined by SDL class definition.
	*/
	template<typename... DeducedArgs>
	static T make(DeducedArgs&&... args);
};

}// end namespace ph

#include "SDL/TSdl.ipp"
