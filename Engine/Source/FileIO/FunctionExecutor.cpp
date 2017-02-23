#include "FileIO/FunctionExecutor.h"
#include "FileIO/InputPacket.h"
#include "Actor/PhysicalActor.h"
#include "Math/TVector3.h"
#include "Math/TQuaternion.h"
#include "FileIO/InputPrototype.h"

#include <string>
#include <iostream>

namespace ph
{

void FunctionExecutor::executeTransform(const InputPacket& packet)
{
	DataTreatment requiredData(EDataImportance::REQUIRED, "FunctionExecutor::executeTransform()");
	PhysicalActor* actor = packet.getPhysicalActor("target", requiredData);
	if(!actor)
	{
		return;
	}

	const std::string typeString = packet.getString("type");
	if(typeString == "translate")
	{
		DataTreatment translateDT(EDataImportance::REQUIRED, requiredData.notFoundInfo + " : translate");
		const Vector3R translation = packet.getVector3r("factor", Vector3R(0, 0, 0), translateDT);
		actor->translate(translation);
	}
	else if(typeString == "rotate")
	{
		InputPrototype quaternionInput;
		quaternionInput.addQuaternionR("factor");

		InputPrototype axisDegreeInput;
		axisDegreeInput.addVector3r("axis");
		axisDegreeInput.addReal("degree");

		if(packet.isPrototypeMatched(quaternionInput))
		{
			const QuaternionR rotation = packet.getQuaternionR("factor");
			actor->rotate(rotation.normalize());
		}
		else if(packet.isPrototypeMatched(axisDegreeInput))
		{
			const Vector3R axis    = packet.getVector3r("axis");
			const real     degrees = packet.getReal("degree");
			actor->rotate(axis.normalize(), degrees);
		}
		else
		{
			std::cerr << "warning: at FunctionExecutor::executeTransform(), cannot find suitable parameters, " << 
			             "acceptable input formats are: \n" << 
			             "1. " << quaternionInput.toString() << "\n" << 
			             "2. " << axisDegreeInput.toString() << std::endl;
		}
	}
	else if(typeString == "scale")
	{
		DataTreatment scaleDT(EDataImportance::REQUIRED, requiredData.notFoundInfo + " : translate");
		const Vector3R scalation = packet.getVector3r("factor", Vector3R(1, 1, 1), scaleDT);
		actor->scale(scalation);
	}
	else
	{
		std::cerr << "warning: at FunctionExecutor::executeTransform(), " << 
		             "unknown type of transform <" << typeString << ">" << std::endl;
	}
}

}// end namespace ph