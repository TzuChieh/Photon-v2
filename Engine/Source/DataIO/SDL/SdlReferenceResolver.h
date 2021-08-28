#pragma once

#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
#include <cstddef>
#include <queue>
#include <optional>

namespace ph { class SceneDescription; }
namespace ph { class ISdlResource; }

namespace ph
{

class SdlReferenceResolver final
{
public:
	SdlReferenceResolver();

	void analyze(const SceneDescription& scene);
	const ISdlResource* dispatch();

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
