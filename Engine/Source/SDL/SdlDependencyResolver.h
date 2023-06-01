#pragma once

#include "Utility/TSpan.h"

#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
#include <cstddef>
#include <queue>
#include <optional>

namespace ph { class ISdlResource; }

namespace ph
{

class SdlDependencyResolver final
{
public:
	SdlDependencyResolver();

	/*! @brief Submit resources and start to resolve their dependencies.
	The containers for input do not need to be kept alive after this call.
	@param resources Resources to be analyzed.
	@param resourceNames Names for the resources. Must have exactly the same size as @p resources if provided.
	*/
	void analyze(
		TSpanView<const ISdlResource*> resources,
		TSpanView<std::string_view> resourceNames = {});

	/*! @brief Get a resource from the analyzed scene with a valid dependency ordering.
	@return A resource. Can be called repeatedly until `nullptr` is returned (which indicates all
	analyzed resources are returned).
	*/
	const ISdlResource* next();

	/*! @brief Get resource name by resource pointer.
	Only valid for the resources in the last `analyze()` call.
	*/
	std::string_view getResourceName(const ISdlResource* resource) const;

private:
	struct ResourceInfo
	{
		const ISdlResource* resource;
		std::string         name;

		inline ResourceInfo() :
			resource(nullptr), name()
		{}
	};

	std::vector<ResourceInfo>                            m_resourceInfos;
	std::queue<const ISdlResource*>                      m_queuedResources;
	std::unordered_map<const ISdlResource*, std::size_t> m_resourceToInfoIndex;

	void calcDispatchOrderFromTopologicalSort();
	std::optional<std::size_t> getResourceInfoIdx(const ISdlResource* resource) const;
};

}// end namespace ph
