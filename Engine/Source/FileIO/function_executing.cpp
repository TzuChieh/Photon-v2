#include "FileIO/function_executing.h"
#include "FileIO/InputPacket.h"

#include <string>
#include <iostream>

namespace ph
{

void execute_transform(const InputPacket& packet)
{
	const std::string typeString = packet.getString("type");
	if(typeString == "translate")
	{
		
	}
	else if(typeString == "rotate")
	{
		
	}
	else if(typeString == "scale")
	{

	}
	else
	{
		std::cerr << "warning: at execute_transform(), unknown type of transform <" << typeString << ">" << std::endl;
	}
}

}// end namespace ph