#pragma once

/*! @file

@brief SDL resource helpers.
*/

#include "DataIO/SDL/ISdlResource.h"
#include "DataIO/SDL/ETypeCategory.h"
#include "DataIO/SDL/sdl_traits.h"

#include <type_traits>
#include <concepts>
#include <memory>

namespace ph
{

template<CIsSdlResource T>
class TSdl final
{
public:
	/*! @brief Statically gets the category of @p T.
	@tparam T Type that category information is going to be extracted from.
	@return Category of @p T. The result is `ETypeCategory::Unspecified` if category
	information does not exist.
	*/
	static constexpr ETypeCategory getCategory();

	/*! @brief Creates a sharable resource filled with default values.
	*/
	static std::shared_ptr<T> makeResource();

	/*! @brief Creates a resource or struct instance filled with default values.
	*/
	static T make();
};

}// end namespace ph

#include "DataIO/SDL/TSdl.ipp"
