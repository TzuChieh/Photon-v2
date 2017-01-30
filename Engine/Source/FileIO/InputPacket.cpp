#include "FileIO/InputPacket.h"
#include "FileIO/NamedResourceCache.h"

#include <iostream>

#define TYPENAME_STRING   "string"
#define TYPENAME_INTEGER  "integer"
#define TYPENAME_REAL     "real"
#define TYPENAME_VECTOR3R "vector3r"

#define TYPENAME_GEOMETRY    "geometry"
#define TYPENAME_TEXTURE     "texture"
#define TYPENAME_MATERIAL    "material"
#define TYPENAME_LIGHTSOURCE "light-source"
#define TYPENAME_ACTOR_MODEL "actor-model"
#define TYPENAME_ACTOR_LIGHT "actor-light"

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
	return findStringValue(TYPENAME_STRING, name, notFoundMessage, &stringValue) ? 
           stringValue : defaultString;
}

integer InputPacket::getInteger(const std::string& name, const integer defaultInteger, const std::string& notFoundMessage) const
{
	std::string stringValue;
	return findStringValue(TYPENAME_INTEGER, name, notFoundMessage, &stringValue) ? 
	       static_cast<integer>(std::stoll(stringValue)) : defaultInteger;
}

real InputPacket::getReal(const std::string& name, const real defaultReal, const std::string& notFoundMessage) const
{
	std::string stringValue;
	return findStringValue(TYPENAME_REAL, name, notFoundMessage, &stringValue) ? 
	       static_cast<real>(std::stold(stringValue)) : defaultReal;
}

Vector3R InputPacket::getVector3R(const std::string& name, const Vector3R& defaultVector3r, const std::string& notFoundMessage) const
{
	std::string stringValue;
	return findStringValue(TYPENAME_VECTOR3R, name, notFoundMessage, &stringValue) ? 
	       Vector3R(stringValue) : defaultVector3r;
}

std::shared_ptr<Geometry> InputPacket::getGeometry(const std::string& name, const std::string& notFoundMessage) const
{
	std::string stringValue;
	return findStringValue(TYPENAME_GEOMETRY, name, notFoundMessage, &stringValue) ? 
	       m_cache.getGeometry(stringValue, notFoundMessage) : nullptr;
}

std::shared_ptr<Texture> InputPacket::getTexture(const std::string& name, const std::string& notFoundMessage) const
{
	std::string stringValue;
	return findStringValue(TYPENAME_TEXTURE, name, notFoundMessage, &stringValue) ?
	                       m_cache.getTexture(stringValue, notFoundMessage) : nullptr;
}

std::shared_ptr<Material> InputPacket::getMaterial(const std::string& name, const std::string& notFoundMessage) const
{
	std::string stringValue;
	return findStringValue(TYPENAME_MATERIAL, name, notFoundMessage, &stringValue) ?
	                       m_cache.getMaterial(stringValue, notFoundMessage) : nullptr;
}

std::shared_ptr<LightSource> InputPacket::getLightSource(const std::string& name, const std::string& notFoundMessage) const
{
	std::string stringValue;
	return findStringValue(TYPENAME_LIGHTSOURCE, name, notFoundMessage, &stringValue) ?
	                       m_cache.getLightSource(stringValue, notFoundMessage) : nullptr;
}

PhysicalActor* InputPacket::getPhysicalActor(const std::string& name, const std::string& notFoundMessage) const
{
	std::string stringValue;
	PhysicalActor* actor = nullptr;
	if(findStringValue(TYPENAME_ACTOR_MODEL, name, "", &stringValue))
	{
		actor = m_cache.getActorModel(stringValue, notFoundMessage);
	}
	else if(findStringValue(TYPENAME_ACTOR_LIGHT, name, "", &stringValue))
	{
		actor = m_cache.getActorLight(stringValue, notFoundMessage);
	}

	if(!actor && !notFoundMessage.empty())
	{
		printNotFoundMessage("physical-actor", name, notFoundMessage);
	}

	return actor;
}

bool InputPacket::findStringValue(const std::string& typeName, const std::string& name, const std::string& notFoundMessage, 
                                  std::string* const out_value) const
{
	for(const ValueClause& vClause : m_vClauses)
	{
		if(vClause.type == typeName && vClause.name == name)
		{
			*out_value = vClause.value;
			return true;
		}
	}

	if(!notFoundMessage.empty())
	{
		printNotFoundMessage(typeName, name, notFoundMessage);
	}

	out_value->clear();
	return false;
}

void InputPacket::printNotFoundMessage(const std::string& typeName, const std::string& name, const std::string& message)
{
	std::cerr << "warning: type<" << typeName << "> name<" << name << "> not found (" << message << ")" << std::endl;
}

}// end namespace ph