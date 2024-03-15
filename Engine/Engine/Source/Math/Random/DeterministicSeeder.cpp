#include "Math/Random/DeterministicSeeder.h"
#include "EngineInitSettings.h"
#include "Math/math_table.h"

#include <Common/config.h>
#include <Common/logging.h>

#include <random>

namespace ph::math
{

std::atomic<uint32> DeterministicSeeder::s_numberSource;
uint32 DeterministicSeeder::s_step;

#if PH_ENSURE_LOCKFREE_ALGORITHMS_ARE_LOCKLESS
static_assert(std::atomic<uint32>::is_always_lock_free);
#endif

void DeterministicSeeder::init(const EngineInitSettings& settings)
{
	if(settings.useRandomSeed)
	{
		std::random_device rd;
		s_numberSource = rd();
		s_step = table::PRIME[rd() % table::PRIME.size()];
	}
	else
	{
		s_numberSource = settings.fixedSeed;
		s_step = settings.fixedSeedStep;
	}

	PH_DEFAULT_LOG(Note, 
		"DeterministicSeeder initialized: using seed value = {}, step = {}",
		s_numberSource.load(), s_step);
}

}// end namespace ph::math
