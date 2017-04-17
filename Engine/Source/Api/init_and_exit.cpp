#include "Api/init_and_exit.h"
#include "Api/ApiDatabase.h"

#include <iostream>

namespace ph
{

bool init_command_parser()
{


	return true;
}

bool exit_api_database()
{
	ApiDatabase::clear();

	return true;
}

}// end namespace ph