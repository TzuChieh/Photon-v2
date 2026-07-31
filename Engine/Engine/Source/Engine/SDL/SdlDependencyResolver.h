#pragma once

#include "Engine/SDL/sdl_fwd.h"
#include "Engine/Utility/TSpan.h"

#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
#include <cstddef>
#include <queue>
#include <optional>

namespace ph
{

/*! @brief Resolve SDL resource dependencies and produce a valid initialization order.
Dependencies always precede resources that reference them. When multiple resources could be
returned next without violating dependency order, the one with the lowest numeric priority is always
selected. Input order breaks equal-priority ties.
*/
class SdlDependencyResolver final
{
public:
	/*! @brief Optional additional information for dependency analysis.
	Each non-empty span must contain one entry per resource passed to `analyze()`. Resource names are
	used for diagnostics and lookup. Among resources that could be returned next, lower numeric
	priorities are selected first and input order breaks equal values.
	*/
	struct AnalysisOptions final
	{
		TSpanView<std::string> resourceNames = {};
		TSpanView<std::size_t> resourcePriorities = {};
	};

	SdlDependencyResolver() = default;

	/*! @brief Submit resources and start to resolve their dependencies.
	The containers for input do not need to be kept alive after this call.
	@param resources Resources to be analyzed.
	@param options Optional additional information for the analysis.
	@throw InvalidArgumentException If a non-empty option span does not match the resource count.
	*/
	void analyze(
		TSpanView<const ISdlResource*> resources,
		AnalysisOptions options = {});

	/*! @brief Get a resource from the analyzed scene with a valid dependency ordering.
	@return The next resource in the resolved dispatch order. Can be called repeatedly until `nullptr`
	is returned. Resources in dependency cycles are not returned.
	*/
	const ISdlResource* next();

	/*! @brief Get resource name by resource pointer.
	Only valid for the resources in the last `analyze()` call. String buffer is backed by the resolver.
	*/
	std::string_view getResourceName(const ISdlResource* resource) const;

private:
	struct ResourceInfo
	{
		const ISdlResource* resource = nullptr;
		std::string         name;
		std::size_t         priority = 0;
	};

	std::vector<ResourceInfo>                            m_resourceInfos;
	std::queue<const ISdlResource*>                      m_queuedResources;
	std::unordered_map<const ISdlResource*, std::size_t> m_resourceToInfoIndex;

	void calcDispatchOrderFromTopologicalSort();
	std::optional<std::size_t> getResourceInfoIdx(const ISdlResource* resource) const;
};

}// end namespace ph
