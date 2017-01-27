#include "FileIO/function_executing.h"
#include "FileIO/InputPacket.h"
#include "Actor/PhysicalActor.h"

#include <string>
#include <iostream>

namespace ph
{

void execute_transform(const InputPacket& packet)
{
	PhysicalActor* actor = packet.getPhysicalActor("target", "execute_transform >> argument target not found");
	if(!actor)
	{
		return;
	}

	const std::string typeString = packet.getString("type");
	if(typeString == "translate")
	{
		const Vector3f translation = packet.getVector3r("factor", Vector3f(0, 0, 0), 
		                                                "execute_transform >> argument factor not found");
		actor->translate(translation);
	}
	else if(typeString == "rotate")
	{
		const Vector3f axis = packet.getVector3r("axis", Vector3f(1, 0, 0),
		                                         "execute_transform >> argument axis not found");
		const real degrees = packet.getReal("degree", 0.0f,
		                                    "execute_transform >> argument degree not found");
		actor->rotate(axis.normalize(), degrees);
	}
	else if(typeString == "scale")
	{
		const Vector3f scalation = packet.getVector3r("factor", Vector3f(1, 1, 1),
		                                              "execute_transform >> argument factor not found");
		actor->scale(scalation);
	}
	else
	{
		std::cerr << "warning: at execute_transform(), unknown type of transform <" << typeString << ">" << std::endl;
	}
}

}// end namespace ph