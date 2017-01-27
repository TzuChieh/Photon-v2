#pragma once

#include "Actor/Geometry/Geometry.h"
#include "Actor/Texture/Texture.h"
#include "Actor/Material/Material.h"
#include "Actor/LightSource/LightSource.h"
#include "Actor/AModel.h"
#include "Actor/ALight.h"

#include <unordered_map>
#include <string>
#include <memory>

namespace ph
{

class NamedResourceCache final
{
public:
	void addGeometry(const std::string& name, const std::shared_ptr<Geometry>& geometry);
	void addTexture(const std::string& name, const std::shared_ptr<Texture>& texture);
	void addMaterial(const std::string& name, const std::shared_ptr<Material>& material);
	void addLightSource(const std::string& name, const std::shared_ptr<LightSource>& lightSource);

	std::shared_ptr<Geometry> getGeometry(const std::string& name) const;
	std::shared_ptr<Texture> getTexture(const std::string& name) const;
	std::shared_ptr<Material> getMaterial(const std::string& name) const;
	std::shared_ptr<LightSource> getLightSource(const std::string& name) const;

	void addActorModel(const std::string& name, std::unique_ptr<AModel> actorModel);
	void addActorLight(const std::string& name, std::unique_ptr<ALight> actorLight);
	const AModel* getActorModel(const std::string& name) const;
	const ALight* getActorLight(const std::string& name) const;

	std::vector<std::unique_ptr<Actor>> claimAllActors();

private:
	std::unordered_map<std::string, std::shared_ptr<Geometry>>    m_geometries;
	std::unordered_map<std::string, std::shared_ptr<Texture>>     m_textures;
	std::unordered_map<std::string, std::shared_ptr<Material>>    m_materials;
	std::unordered_map<std::string, std::shared_ptr<LightSource>> m_lightSources;

	std::unordered_map<std::string, std::unique_ptr<AModel>> m_actorModels;
	std::unordered_map<std::string, std::unique_ptr<ALight>> m_actorLights;
};

}// end namespace ph