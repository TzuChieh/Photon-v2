#include "FileIO/function_executing.h"
#include "FileIO/InputPacket.h"
#include "Actor/PhysicalActor.h"

#include <string>
#include <iostream>

namespace ph
{

void execute_transform(const InputPacket& packet)
{
	PhysicalActor* actor = packet.getPhysicalActor("target", "at execute_transform()");
	if(!actor)
	{
		return;
	}

	const std::string typeString = packet.getString("type");
	if(typeString == "translate")
	{
		const Vector3f translation = packet.getVector3r("factor", Vector3f(0, 0, 0), "at execute_transform(): translate");
		actor->translate(translation);
	}
	else if(typeString == "rotate")
	{
		const Vector3f axis = packet.getVector3r("axis", Vector3f(1, 0, 0), "at execute_transform(): rotate");
		const real degrees = packet.getReal("degree", 0.0f, "at execute_transform(): rotate");
		actor->rotate(axis.normalize(), degrees);
	}
	else if(typeString == "scale")
	{
		const Vector3f scalation = packet.getVector3r("factor", Vector3f(1, 1, 1), "at execute_transform(): scale");
		actor->scale(scalation);
	}
	else
	{
		std::cerr << "warning: at execute_transform(), unknown type of transform <" << typeString << ">" << std::endl;
	}
}

}// end namespace ph