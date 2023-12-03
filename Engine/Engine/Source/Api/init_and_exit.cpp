#include "Api/init_and_exit.h"
#include "Api/ApiDatabase.h"
#include "Common/logging.h"
#include "DataIO/io_utils.h"

#include <iostream>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(InitAndExit, Engine);

bool init_engine_IO_infrastructure()
{
	if(!io_utils::detail::init_picture_IO())
	{
		PH_LOG_WARNING(InitAndExit,
			"init_picture_IO() failed");
		return false;
	}

	return true;
}

bool exit_API_database()
{
	ApiDatabase::clear();

	return true;
}

}// end namespace ph