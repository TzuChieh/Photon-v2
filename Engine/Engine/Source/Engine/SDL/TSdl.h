#pragma once

/*! @file

@brief SDL instance helpers.
*/

#include "Engine/SDL/ESdlTypeCategory.h"
#include "Engine/SDL/sdl_traits.h"

#include <type_traits>
#include <concepts>
#include <memory>

namespace ph
{

class Path;
class ISdlResource;
class SdlClass;

namespace detail
{

std::shared_ptr<ISdlResource> load_single_resource(const SdlClass* resourceClass, const Path& file);
void save_single_resource(const std::shared_ptr<ISdlResource>& resource, const Path& file);

}// end namespace detail

template<typename StaticT = void>
class TSdl final
{};

/*! @brief `TSdl` specialization that accepts dynamic type (`SdlClass`).
*/
template<>
class TSdl<void> final
{
public:
	/*! @brief Creates a sharable resource filled with default values.
	Default values are determined by SDL class definition.
	*/
	static std::shared_ptr<ISdlResource> makeResource(const SdlClass* clazz);
};

/*! @brief `TSdl` specialization that accepts an SDL resource type.
*/
template<CSdlResource T>
class TSdl<T> final
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

	/*! @brief Creates a resource instance filled with default values.
	Default values are determined by SDL class definition.
	*/
	template<typename... DeducedArgs>
	static T make(DeducedArgs&&... args);

	/*! @brief Loads a single resource from file.
	The file is assumed to contain only 1 resource.
	*/
	static std::shared_ptr<T> loadResource(const Path& file);

	/*! @brief Saves a single resource to file.
	*/
	static void saveResource(const std::shared_ptr<T>& resource, const Path& file);
};

/*! @brief `TSdl` specialization that accepts an SDL struct type.
*/
template<CSdlStruct T>
class TSdl<T> final
{
public:
	/*! @brief Creates a struct instance filled with default values.
	Default values are determined by SDL struct definition.
	*/
	template<typename... DeducedArgs>
	static T make(DeducedArgs&&... args);
};

}// end namespace ph

#include "Engine/SDL/TSdl.ipp"
