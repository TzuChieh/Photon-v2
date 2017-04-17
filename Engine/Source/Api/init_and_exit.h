#pragma once

namespace ph
{

extern bool init_command_parser();

extern bool exit_api_database();

template<typename T>
void register_command_interface()
{
	T::registerInterface();
}

}// end namespace ph