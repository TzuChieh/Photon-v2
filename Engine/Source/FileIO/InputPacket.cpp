#include "FileIO/InputPacket.h"
#include "FileIO/NamedResourceCache.h"
#include "FileIO/ValueParser.h"
#include "FileIO/Keyword.h"
#include "FileIO/InputPrototype.h"

#include <iostream>

namespace ph
{

InputPacket::InputPacket(const std::vector<ValueClause>& vClauses, const NamedResourceCache& cache) :
	m_vClauses(vClauses), m_cache(cache)
{

}

InputPacket::InputPacket(InputPacket&& other) : 
	m_vClauses(std::move(other.m_vClauses)), m_cache(other.m_cache)
{

}

std::string InputPacket::getString(const std::string& name, const std::string& defaultString, const std::string& notFoundMessage) const
{
	std::string stringValue;
	return findStringValue(Keyword::TYPENAME_STRING, name, notFoundMessage, &stringValue) ?
	       ValueParser::parseString(stringValue) : defaultString;
}

integer InputPacket::getInteger(const std::string& name, const integer defaultInteger, const std::string& notFoundMessage) const
{
	std::string stringValue;
	return findStringValue(Keyword::TYPENAME_INTEGER, name, notFoundMessage, &stringValue) ?
	       ValueParser::parseInteger(stringValue) : defaultInteger;
}

real InputPacket::getReal(const std::string& name, const real defaultReal, const std::string& notFoundMessage) const
{
	std::string stringValue;
	return findStringValue(Keyword::TYPENAME_REAL, name, notFoundMessage, &stringValue) ?
	       ValueParser::parseReal(stringValue) : defaultReal;
}

Vector3R InputPacket::getVector3r(const std::string& name,
                                  const Vector3R& defaultVector3r, 
                                  const std::string& notFoundMessage) const
{
	std::string stringValue;
	return findStringValue(Keyword::TYPENAME_VECTOR3R, name, notFoundMessage, &stringValue) ?
	       ValueParser::parseVector3r(stringValue) : defaultVector3r;
}

QuaternionR InputPacket::getQuaternionR(const std::string& name,
                                        const QuaternionR& defaultQuaternionR,
                                        const std::string& notFoundMessage) const
{
	std::string stringValue;
	return findStringValue(Keyword::TYPENAME_QUATERNIONR, name, notFoundMessage, &stringValue) ?
	       ValueParser::parseQuaternionR(stringValue) : defaultQuaternionR;
}

std::vector<Vector3R> InputPacket::getVector3rArray(const std::string& name,
                                                    const std::vector<Vector3R>& defaultVector3rArray,
                                                    const std::string& notFoundMessage) const
{
	std::string stringValue;
	return findStringValue(Keyword::TYPENAME_VECTOR3R_ARRAY, name, notFoundMessage, &stringValue) ?
	       ValueParser::parseVector3rArray(stringValue) : defaultVector3rArray;
}

std::shared_ptr<Geometry> InputPacket::getGeometry(const std::string& name, const std::string& notFoundMessage) const
{
	std::string stringValue;
	return findStringValue(Keyword::TYPENAME_GEOMETRY, name, notFoundMessage, &stringValue) ?
	       m_cache.getGeometry(stringValue, notFoundMessage) : nullptr;
}

std::shared_ptr<Texture> InputPacket::getTexture(const std::string& name, const std::string& notFoundMessage) const
{
	std::string stringValue;
	return findStringValue(Keyword::TYPENAME_TEXTURE, name, notFoundMessage, &stringValue) ?
	                       m_cache.getTexture(stringValue, notFoundMessage) : nullptr;
}

std::shared_ptr<Material> InputPacket::getMaterial(const std::string& name, const std::string& notFoundMessage) const
{
	std::string stringValue;
	return findStringValue(Keyword::TYPENAME_MATERIAL, name, notFoundMessage, &stringValue) ?
	                       m_cache.getMaterial(stringValue, notFoundMessage) : nullptr;
}

std::shared_ptr<LightSource> InputPacket::getLightSource(const std::string& name, const std::string& notFoundMessage) const
{
	std::string stringValue;
	return findStringValue(Keyword::TYPENAME_LIGHTSOURCE, name, notFoundMessage, &stringValue) ?
	                       m_cache.getLightSource(stringValue, notFoundMessage) : nullptr;
}

PhysicalActor* InputPacket::getPhysicalActor(const std::string& name, const std::string& notFoundMessage) const
{
	std::string stringValue;
	PhysicalActor* actor = nullptr;
	if(findStringValue(Keyword::TYPENAME_ACTOR_MODEL, name, "", &stringValue))
	{
		actor = m_cache.getActorModel(stringValue, notFoundMessage);
	}
	else if(findStringValue(Keyword::TYPENAME_ACTOR_LIGHT, name, "", &stringValue))
	{
		actor = m_cache.getActorLight(stringValue, notFoundMessage);
	}

	if(!actor && !notFoundMessage.empty())
	{
		printNotFoundMessage("physical-actor", name, notFoundMessage);
	}

	return actor;
}

bool InputPacket::isPrototypeMatched(const InputPrototype& prototype) const
{
	for(const auto& typeNamePair : prototype.typeNamePairs)
	{
		const std::string& typeString = typeNamePair.first;
		const std::string& nameString = typeNamePair.second;
		if(!findStringValue(typeString, nameString, "", nullptr))
		{
			return false;
		}
	}

	return true;
}

bool InputPacket::findStringValue(const std::string& typeName, const std::string& name, const std::string& notFoundMessage, 
                                  std::string* const out_value) const
{
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

	if(!notFoundMessage.empty())
	{
		printNotFoundMessage(typeName, name, notFoundMessage);
	}

	if(out_value)
	{
		out_value->clear();
	}

	return false;
}

void InputPacket::printNotFoundMessage(const std::string& typeName, const std::string& name, const std::string& message)
{
	std::cerr << "warning: type<" << typeName << "> name<" << name << "> not found (" << message << ")" << std::endl;
}

}// end namespace ph