#include "Api/init_and_exit.h"
#include "Api/ApiDatabase.h"
#include "DataIO/io_utils.h"
#include "Core/LTA/SurfaceHitRefinery.h"
#include "Core/LTA/SurfaceHitRefinery.h"

#include <Common/config.h>
#include <Common/logging.h>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(InitAndExit, Engine);

bool init_engine_core(const EngineInitSettings& settings)
{
	lta::SurfaceHitRefinery::init(settings);

	return true;
}

bool init_engine_IO_infrastructure(const EngineInitSettings& settings)
{
	if(!io_utils::detail::init_picture_IO())
	{
		PH_LOG(InitAndExit, Warning,
			"init_picture_IO() failed");
		return false;
	}

	return true;
}

void after_engine_init(const EngineInitSettings& settings)
{}

void before_engine_exit()
{
#if PH_ENABLE_HIT_EVENT_STATS
	lta::SurfaceHitRefinery::reportStats();
#endif
}

bool exit_API_database()
{
	ApiDatabase::clear();

	return true;
}

}// end namespace ph
