#pragma once

/*! @file

@brief SDL resource helpers.
*/

#include "DataIO/SDL/ISdlResource.h"
#include "DataIO/SDL/ETypeCategory.h"
#include "DataIO/SDL/sdl_helpers.h"

#include <type_traits>
#include <concepts>
#include <memory>

namespace ph
{

template<sdl::CIsResource T>
class TSdl final
{
public:
	/*! @brief Statically gets the category of @p T.
	@tparam T Type that category information is going to be extracted from.
	@return Category of @p T. The result is `ETypeCategory::UNSPECIFIED` if category
	information does not exist.
	*/
	static constexpr ETypeCategory getCategory();

	/*! @brief
	*/
	static std::shared_ptr<T> makeResource();
};


}// end namespace ph

#include "DataIO/SDL/TSdl.ipp"
