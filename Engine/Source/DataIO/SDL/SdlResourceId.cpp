#include "DataIO/SDL/SdlResourceId.h"

#include <atomic>

namespace ph
{

SdlResourceId gen_sdl_resource_id()
{
	static std::atomic<SdlResourceId> counter(0);

	// Pre-increment, dispatched ID will start from 1 (0 reserved for special purposes)
	return ++counter;
}

}// end namespace ph
