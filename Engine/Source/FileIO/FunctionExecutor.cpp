#include "FileIO/FunctionExecutor.h"
#include "FileIO/InputPacket.h"
#include "Actor/PhysicalActor.h"
#include "Math/TVector3.h"

#include <string>
#include <iostream>

namespace ph
{

void FunctionExecutor::executeTransform(const InputPacket& packet)
{
	PhysicalActor* actor = packet.getPhysicalActor("target", "at execute_transform()");
	if(!actor)
	{
		return;
	}

	const std::string typeString = packet.getString("type");
	if(typeString == "translate")
	{
		const Vector3R translation = packet.getVector3R("factor", Vector3R(0, 0, 0), "at execute_transform(): translate");
		actor->translate(translation);
	}
	else if(typeString == "rotate")
	{
		const Vector3R axis = packet.getVector3R("axis", Vector3R(1, 0, 0), "at execute_transform(): rotate");
		const real degrees = packet.getReal("degree", 0.0f, "at execute_transform(): rotate");
		actor->rotate(axis.normalize(), degrees);
	}
	else if(typeString == "scale")
	{
		const Vector3R scalation = packet.getVector3R("factor", Vector3R(1, 1, 1), "at execute_transform(): scale");
		actor->scale(scalation);
	}
	else
	{
		std::cerr << "warning: at execute_transform(), unknown type of transform <" << typeString << ">" << std::endl;
	}
}

}// end namespace ph