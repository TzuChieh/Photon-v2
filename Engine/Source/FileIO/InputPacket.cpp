#include "FileIO/InputPacket.h"
#include "FileIO/NamedResourceStorage.h"
#include "FileIO/ValueParser.h"
#include "FileIO/Keyword.h"
#include "FileIO/InputPrototype.h"

#include <iostream>

namespace ph
{

InputPacket::InputPacket(const std::vector<ValueClause>& vClauses, const NamedResourceStorage& storage) : 
	m_vClauses(vClauses), m_storage(storage)
{

}

InputPacket::InputPacket(InputPacket&& other) : 
	m_vClauses(std::move(other.m_vClauses)), m_storage(other.m_storage)
{

}

std::string InputPacket::getString(
	const std::string& name, 
	const std::string& defaultString, 
	const DataTreatment& treatment) const
{
	std::string stringValue;
	return findStringValue(Keyword::TYPENAME_STRING, name, treatment, &stringValue) ?
	       ValueParser::parseString(stringValue) : defaultString;
}

integer InputPacket::getInteger(
	const std::string& name, 
	const integer defaultInteger, 
	const DataTreatment& treatment) const
{
	std::string stringValue;
	return findStringValue(Keyword::TYPENAME_INTEGER, name, treatment, &stringValue) ?
	       ValueParser::parseInteger(stringValue) : defaultInteger;
}

real InputPacket::getReal(
	const std::string& name, 
	const real defaultReal, 
	const DataTreatment& treatment) const
{
	std::string stringValue;
	return findStringValue(Keyword::TYPENAME_REAL, name, treatment, &stringValue) ?
	       ValueParser::parseReal(stringValue) : defaultReal;
}

Vector3R InputPacket::getVector3r(
	const std::string& name,
	const Vector3R& defaultVector3r, 
	const DataTreatment& treatment) const
{
	std::string stringValue;
	return findStringValue(Keyword::TYPENAME_VECTOR3R, name, treatment, &stringValue) ?
	       ValueParser::parseVector3r(stringValue) : defaultVector3r;
}

QuaternionR InputPacket::getQuaternionR(
	const std::string& name,
	const QuaternionR& defaultQuaternionR,
	const DataTreatment& treatment) const
{
	std::string stringValue;
	return findStringValue(Keyword::TYPENAME_QUATERNIONR, name, treatment, &stringValue) ?
	       ValueParser::parseQuaternionR(stringValue) : defaultQuaternionR;
}

std::vector<Vector3R> InputPacket::getVector3rArray(
	const std::string& name,
	const std::vector<Vector3R>& defaultVector3rArray,
	const DataTreatment& treatment) const
{
	std::string stringValue;
	return findStringValue(Keyword::TYPENAME_VECTOR3R_ARRAY, name, treatment, &stringValue) ?
	       ValueParser::parseVector3rArray(stringValue) : defaultVector3rArray;
}

std::shared_ptr<Geometry> InputPacket::getGeometry(
	const std::string& name, 
	const DataTreatment& treatment) const
{
	std::string stringValue;
	return findStringValue(Keyword::TYPENAME_GEOMETRY, name, treatment, &stringValue) ?
	                       m_storage.getGeometry(stringValue, treatment) : nullptr;
}

std::shared_ptr<Texture> InputPacket::getTexture(
	const std::string& name, 
	const DataTreatment& treatment) const
{
	std::string stringValue;
	return findStringValue(Keyword::TYPENAME_TEXTURE, name, treatment, &stringValue) ?
	                       m_storage.getTexture(stringValue, treatment) : nullptr;
}

std::shared_ptr<Material> InputPacket::getMaterial(
	const std::string& name, 
	const DataTreatment& treatment) const
{
	std::string stringValue;
	return findStringValue(Keyword::TYPENAME_MATERIAL, name, treatment, &stringValue) ?
	                       m_storage.getMaterial(stringValue, treatment) : nullptr;
}

std::shared_ptr<LightSource> InputPacket::getLightSource(
	const std::string& name, 
	const DataTreatment& treatment) const
{
	std::string stringValue;
	return findStringValue(Keyword::TYPENAME_LIGHTSOURCE, name, treatment, &stringValue) ?
	                       m_storage.getLightSource(stringValue, treatment) : nullptr;
}

std::shared_ptr<PhysicalActor> InputPacket::getPhysicalActor(
	const std::string& name, 
	const DataTreatment& treatment) const
{
	std::string stringValue;
	std::shared_ptr<PhysicalActor> actor;
	if(findStringValue(Keyword::TYPENAME_ACTOR_MODEL, name, DataTreatment(), &stringValue))
	{
		actor = m_storage.getActorModel(stringValue, treatment);
	}
	else if(findStringValue(Keyword::TYPENAME_ACTOR_LIGHT, name, DataTreatment(), &stringValue))
	{
		actor = m_storage.getActorLight(stringValue, treatment);
	}

	if(!actor)
	{
		// HACK: hard-coded value
		reportDataNotFound("physical-actor", name, treatment);
	}

	return actor;
}

bool InputPacket::isPrototypeMatched(const InputPrototype& prototype) const
{
	for(const auto& typeNamePair : prototype.typeNamePairs)
	{
		const std::string& typeString = typeNamePair.first;
		const std::string& nameString = typeNamePair.second;
		if(!findStringValue(typeString, nameString, DataTreatment(), nullptr))
		{
			return false;
		}
	}

	return true;
}

bool InputPacket::findStringValue(const std::string& typeName, const std::string& name, const DataTreatment& treatment, 
                                  std::string* const out_value) const
{
	if(out_value)
	{
		out_value->clear();
	}

	for(const ValueClause& vClause : m_vClauses)
	{
		if(vClause.type == typeName && vClause.name == name)
		{
			if(out_value)
			{
				*out_value = vClause.value;
			}

			return true;
		}
	}

	reportDataNotFound(typeName, name, treatment);
	return false;
}

void InputPacket::reportDataNotFound(const std::string& typeName, const std::string& name, const DataTreatment& treatment)
{
	const std::string& message = treatment.notFoundInfo;

	switch(treatment.importance)
	{
	case EDataImportance::OPTIONAL:
		if(!message.empty())
		{
			std::cerr << "warning: optional data type<" << typeName << "> name<" << name << "> not found (" << message << ")" << std::endl;
		}
		break;

	case EDataImportance::REQUIRED:
		std::cerr << "warning: required data type<" << typeName << "> name<" << name << "> not found ";
		if(!message.empty())
		{
			std::cerr << "(" << message << ")" << std::endl;
		}
		std::cerr << std::endl;
		break;
	}
}

}// end namespace ph