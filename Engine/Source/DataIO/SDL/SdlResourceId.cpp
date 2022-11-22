#include "DataIO/SDL/SdlResourceId.h"

#include <atomic>

namespace ph
{

SdlResourceId gen_sdl_resource_id()
{
	// Dispatched ID starts from 1 (0 reserved for special purposes)
	static std::atomic<SdlResourceId> counter(1);

	return counter.fetch_add(1, std::memory_order_relaxed)
}

}// end namespace ph
