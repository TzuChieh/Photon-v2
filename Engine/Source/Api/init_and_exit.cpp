#include "Api/init_and_exit.h"
#include "Api/ApiDatabase.h"

#include "Actor/Geometry/GSphere.h"

#include <iostream>

namespace ph
{

template<typename T>
void register_command_interface()
{
	TCommandInterface<T>::registerInterface();
}

bool init_command_parser()
{
	register_command_interface<GSphere>();

	return true;
}

bool exit_api_database()
{
	ApiDatabase::clear();

	return true;
}

}// end namespace ph