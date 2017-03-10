#include "FileIO/NamedResourceCache.h"
#include "FileIO/Keyword.h"

#include <iostream>

namespace ph
{

void NamedResourceCache::addGeometry(const std::string& name, const std::shared_ptr<Geometry>& geometry)
{
	if(getGeometry(name))
		std::cerr << "warning: at NamedResourceCache::addGeometry(), name <" + name + "> duplicated, overwriting" << std::endl;

	m_geometries[name] = geometry;
}

void NamedResourceCache::addTexture(const std::string& name, const std::shared_ptr<Texture>& texture)
{
	if(getTexture(name))
		std::cerr << "warning: at NamedResourceCache::addTexture(), name <" + name + "> duplicated, overwriting" << std::endl;

	m_textures[name] = texture;
}

void NamedResourceCache::addMaterial(const std::string& name, const std::shared_ptr<Material>& material)
{
	if(getMaterial(name))
		std::cerr << "warning: at NamedResourceCache::addMaterial(), name <" + name + "> duplicated, overwriting" << std::endl;

	m_materials[name] = material;
}

void NamedResourceCache::addLightSource(const std::string& name, const std::shared_ptr<LightSource>& lightSource)
{
	if(getLightSource(name))
		std::cerr << "warning: at NamedResourceCache::addLightSource(), name <" + name + "> duplicated, overwriting" << std::endl;

	m_lightSources[name] = lightSource;
}

void NamedResourceCache::addActorModel(const std::string& name, std::unique_ptr<AModel> actorModel)
{
	if(getActorModel(name))
		std::cerr << "warning: at NamedResourceCache::addActorModel(), name <" + name + "> duplicated, overwriting" << std::endl;

	m_actorModels[name] = std::move(actorModel);
}

void NamedResourceCache::addActorLight(const std::string& name, std::unique_ptr<ALight> actorLight)
{
	if(getActorLight(name))
		std::cerr << "warning: at NamedResourceCache::addActorLight(), name <" + name + "> duplicated, overwriting" << std::endl;

	m_actorLights[name] = std::move(actorLight);
}

std::shared_ptr<Geometry> NamedResourceCache::getGeometry(const std::string& name, const DataTreatment& treatment) const
{
	if(m_geometries.count(name) == 1)
	{
		return m_geometries.at(name);
	}
	else
	{
		reportResourceNotFound(Keyword::TYPENAME_GEOMETRY, name, treatment);
		return nullptr;
	}
}

std::shared_ptr<Texture> NamedResourceCache::getTexture(const std::string& name, const DataTreatment& treatment) const
{
	if(m_textures.count(name) == 1)
	{
		return m_textures.at(name);
	}
	else
	{
		reportResourceNotFound(Keyword::TYPENAME_TEXTURE, name, treatment);
		return nullptr;
	}
}

std::shared_ptr<Material> NamedResourceCache::getMaterial(const std::string& name, const DataTreatment& treatment) const
{
	if(m_materials.count(name) == 1)
	{
		return m_materials.at(name);
	}
	else
	{
		reportResourceNotFound(Keyword::TYPENAME_MATERIAL, name, treatment);
		return nullptr;
	}
}

std::shared_ptr<LightSource> NamedResourceCache::getLightSource(const std::string& name, const DataTreatment& treatment) const
{
	if(m_lightSources.count(name) == 1)
	{
		return m_lightSources.at(name);
	}
	else
	{
		reportResourceNotFound(Keyword::TYPENAME_LIGHTSOURCE, name, treatment);
		return nullptr;
	}
}

AModel* NamedResourceCache::getActorModel(const std::string& name, const DataTreatment& treatment) const
{
	if(m_actorModels.count(name) == 1)
	{
		return m_actorModels.at(name).get();
	}
	else
	{
		reportResourceNotFound(Keyword::TYPENAME_ACTOR_MODEL, name, treatment);
		return nullptr;
	}
}

ALight* NamedResourceCache::getActorLight(const std::string& name, const DataTreatment& treatment) const
{
	if(m_actorLights.count(name) == 1)
	{
		return m_actorLights.at(name).get();
	}
	else
	{
		reportResourceNotFound(Keyword::TYPENAME_ACTOR_LIGHT, name, treatment);
		return nullptr;
	}
}

std::vector<std::unique_ptr<Actor>> NamedResourceCache::claimAllActors()
{
	std::vector<std::unique_ptr<Actor>> actors;

	for(auto& keyValuePair : m_actorModels)
	{
		actors.push_back(std::move(keyValuePair.second));
	}

	for(auto& keyValuePair : m_actorLights)
	{
		actors.push_back(std::move(keyValuePair.second));
	}

	m_actorModels.clear();
	m_actorLights.clear();

	return actors;
}

void NamedResourceCache::reportResourceNotFound(const std::string& typeName, const std::string& name, const DataTreatment& treatment)
{
	const std::string& message = treatment.notFoundInfo;

	switch(treatment.importance)
	{
	case EDataImportance::OPTIONAL:
		if(!message.empty())
		{
			std::cerr << "warning: optional resource type<" << typeName << "> name<" << name << "> not found (" << message << ")" << std::endl;
		}
		break;

	case EDataImportance::REQUIRED:
		std::cerr << "warning: required resource type<" << typeName << "> name<" << name << "> not found ";
		if(!message.empty())
		{
			std::cerr << "(" << message << ")" << std::endl;
		}
		std::cerr << std::endl;
		break;
	}
}

}// end namespace ph