#include "FileIO/NamedResourceCache.h"

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

std::shared_ptr<Geometry> NamedResourceCache::getGeometry(const std::string& name) const
{
	return m_geometries.count(name) == 1 ? m_geometries.at(name) : nullptr;
}

std::shared_ptr<Texture> NamedResourceCache::getTexture(const std::string& name) const
{
	return m_textures.count(name) == 1 ? m_textures.at(name) : nullptr;
}

std::shared_ptr<Material> NamedResourceCache::getMaterial(const std::string& name) const
{
	return m_materials.count(name) == 1 ? m_materials.at(name) : nullptr;
}

std::shared_ptr<LightSource> NamedResourceCache::getLightSource(const std::string& name) const
{
	return m_lightSources.count(name) == 1 ? m_lightSources.at(name) : nullptr;
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

const AModel* NamedResourceCache::getActorModel(const std::string& name) const
{
	return m_actorModels.count(name) == 1 ? m_actorModels.at(name).get() : nullptr;
}

const ALight* NamedResourceCache::getActorLight(const std::string& name) const
{
	return m_actorLights.count(name) == 1 ? m_actorLights.at(name).get() : nullptr;
}

std::unique_ptr<AModel> NamedResourceCache::claimActorModel(const std::string& name)
{
	if(m_actorModels.count(name) == 1)
	{
		std::unique_ptr<AModel> claimedActorModel = std::move(m_actorModels[name]);
		m_actorModels.erase(name);
		return claimedActorModel;
	}
	else
	{
		return nullptr;
	}
}

std::unique_ptr<ALight> NamedResourceCache::claimActorLight(const std::string& name)
{
	if(m_actorLights.count(name) == 1)
	{
		std::unique_ptr<ALight> claimedActorLight = std::move(m_actorLights[name]);
		m_actorLights.erase(name);
		return claimedActorLight;
	}
	else
	{
		return nullptr;
	}
}

}// end namespace ph