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

Vector3f InputPacket::getVector3r(const std::string& name, const Vector3f& defaultVector3r, const std::string& notFoundMessage) const
{
	std::string stringValue;
	return findStringValue(TYPENAME_VECTOR3R, name, notFoundMessage, &stringValue) ? 
	       Vector3f(stringValue) : defaultVector3r;
}

std::shared_ptr<Geometry> InputPacket::getGeometry(const std::string& name, const std::string& notFoundMessage) const
{
	std::string stringValue;
	return findStringValue(TYPENAME_GEOMETRY, name, notFoundMessage, &stringValue) ? 
	       m_cache.getGeometry(stringValue) : nullptr;
}

std::shared_ptr<Texture> InputPacket::getTexture(const std::string& name, const std::string& notFoundMessage) const
{
	std::string stringValue;
	return findStringValue(TYPENAME_TEXTURE, name, notFoundMessage, &stringValue) ?
	                       m_cache.getTexture(stringValue) : nullptr;
}

std::shared_ptr<Material> InputPacket::getMaterial(const std::string& name, const std::string& notFoundMessage) const
{
	std::string stringValue;
	return findStringValue(TYPENAME_MATERIAL, name, notFoundMessage, &stringValue) ?
	                       m_cache.getMaterial(stringValue) : nullptr;
}

std::shared_ptr<LightSource> InputPacket::getLightSource(const std::string& name, const std::string& notFoundMessage) const
{
	std::string stringValue;
	return findStringValue(TYPENAME_LIGHTSOURCE, name, notFoundMessage, &stringValue) ?
	                       m_cache.getLightSource(stringValue) : nullptr;
}

PhysicalActor* InputPacket::getPhysicalActor(const std::string& name, const std::string& notFoundMessage) const
{
	std::string stringValue;
	if(findStringValue(TYPENAME_ACTOR_MODEL, name, "", &stringValue))
	{
		return m_cache.getActorModel(stringValue);
	}
	else if(findStringValue(TYPENAME_ACTOR_LIGHT, name, "", &stringValue))
	{
		return m_cache.getActorLight(stringValue);
	}
	else
	{
		if(!notFoundMessage.empty())
		{
			printNotFoundMessage(notFoundMessage);
		}

		return nullptr;
	}
}

bool InputPacket::findStringValue(const std::string& type, const std::string& name, const std::string& notFoundMessage, 
                                  std::string* const out_value) const
{
	for(const ValueClause& vClause : m_vClauses)
	{
		if(vClause.type == type && vClause.name == name)
		{
			*out_value = vClause.value;
			return true;
		}
	}

	if(!notFoundMessage.empty())
	{
		printNotFoundMessage(notFoundMessage);
	}

	out_value->clear();
	return false;
}

void InputPacket::printNotFoundMessage(const std::string& notFoundMessage) const
{
	std::cerr << "warning: " << notFoundMessage << std::endl;
}

}// end namespace ph