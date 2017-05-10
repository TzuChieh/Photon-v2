#include "FileIO/NamedResourceStorage.h"
#include "FileIO/Keyword.h"

#include <iostream>

namespace ph
{

NamedResourceStorage::NamedResourceStorage() : 
	m_resources(static_cast<std::size_t>(ETypeCategory::MAX) + 1)
{

}

void NamedResourceStorage::addResource(
	const SdlTypeInfo& typeInfo,
	const std::string& resourceName, 
	const std::shared_ptr<ISdlResource>& resource)
{
	if(resource == nullptr || resourceName.empty())
	{
		std::cerr << "warning: at NamedResourceStorage::addResource(), name <" << resourceName << "> or resource is null, ignoring" << std::endl;
		return;
	}

	const std::size_t categoryIndex = static_cast<std::size_t>(typeInfo.typeCategory);
	if(!checkCategoryIndex(categoryIndex))
	{
		return;
	}

	auto& resourcesNameMap = m_resources[categoryIndex];
	const auto& iter = resourcesNameMap.find(resourceName);
	if(iter != resourcesNameMap.end())
	{
		std::cerr << "warning: at NamedResourceStorage::addResource(), name <" << resourceName << "> duplicated, overwriting" << std::endl;
	}

	resourcesNameMap[resourceName] = resource;
}

std::shared_ptr<ISdlResource> NamedResourceStorage::getResource(
	const SdlTypeInfo& typeInfo,
	const std::string& resourceName,
	const DataTreatment& treatment) const
{
	const std::size_t categoryIndex = static_cast<std::size_t>(typeInfo.typeCategory);
	if(!checkCategoryIndex(categoryIndex))
	{
		return nullptr;
	}

	const auto& resourcesNameMap = m_resources[categoryIndex];
	const auto& iter = resourcesNameMap.find(resourceName);
	if(iter == resourcesNameMap.end())
	{
		reportResourceNotFound(SdlTypeInfo::categoryToName(typeInfo.typeCategory), resourceName, treatment);
		return nullptr;
	}

	return iter->second;
}

void NamedResourceStorage::addGeometry(const std::string& name, const std::shared_ptr<Geometry>& geometry)
{
	if(getGeometry(name))
		std::cerr << "warning: at NamedResourceStorage::addGeometry(), name <" + name + "> duplicated, overwriting" << std::endl;

	m_geometries[name] = geometry;
}

void NamedResourceStorage::addTexture(const std::string& name, const std::shared_ptr<Texture>& texture)
{
	if(getTexture(name))
		std::cerr << "warning: at NamedResourceStorage::addTexture(), name <" + name + "> duplicated, overwriting" << std::endl;

	m_textures[name] = texture;
}

void NamedResourceStorage::addMaterial(const std::string& name, const std::shared_ptr<Material>& material)
{
	if(getMaterial(name))
		std::cerr << "warning: at NamedResourceStorage::addMaterial(), name <" + name + "> duplicated, overwriting" << std::endl;

	m_materials[name] = material;
}

void NamedResourceStorage::addLightSource(const std::string& name, const std::shared_ptr<LightSource>& lightSource)
{
	if(getLightSource(name))
		std::cerr << "warning: at NamedResourceStorage::addLightSource(), name <" + name + "> duplicated, overwriting" << std::endl;

	m_lightSources[name] = lightSource;
}

void NamedResourceStorage::addActorModel(const std::string& name, std::shared_ptr<AModel> actorModel)
{
	if(getActorModel(name))
		std::cerr << "warning: at NamedResourceStorage::addActorModel(), name <" + name + "> duplicated, overwriting" << std::endl;

	m_actorModels[name] = actorModel;
}

void NamedResourceStorage::addActorLight(const std::string& name, std::shared_ptr<ALight> actorLight)
{
	if(getActorLight(name))
		std::cerr << "warning: at NamedResourceStorage::addActorLight(), name <" + name + "> duplicated, overwriting" << std::endl;

	m_actorLights[name] = actorLight;
}

std::shared_ptr<Geometry> NamedResourceStorage::getGeometry(const std::string& name, const DataTreatment& treatment) const
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

std::shared_ptr<Texture> NamedResourceStorage::getTexture(const std::string& name, const DataTreatment& treatment) const
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

std::shared_ptr<Material> NamedResourceStorage::getMaterial(const std::string& name, const DataTreatment& treatment) const
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

std::shared_ptr<LightSource> NamedResourceStorage::getLightSource(const std::string& name, const DataTreatment& treatment) const
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

std::shared_ptr<AModel> NamedResourceStorage::getActorModel(const std::string& name, const DataTreatment& treatment) const
{
	if(m_actorModels.count(name) == 1)
	{
		return m_actorModels.at(name);
	}
	else
	{
		reportResourceNotFound(Keyword::TYPENAME_ACTOR_MODEL, name, treatment);
		return nullptr;
	}
}

std::shared_ptr<ALight> NamedResourceStorage::getActorLight(const std::string& name, const DataTreatment& treatment) const
{
	if(m_actorLights.count(name) == 1)
	{
		return m_actorLights.at(name);
	}
	else
	{
		reportResourceNotFound(Keyword::TYPENAME_ACTOR_LIGHT, name, treatment);
		return nullptr;
	}
}

std::vector<std::shared_ptr<Actor>> NamedResourceStorage::getActors() const
{
	std::vector<std::shared_ptr<Actor>> actors;

	const std::size_t actorCategoryIndex = static_cast<std::size_t>(Actor::ciTypeInfo().typeCategory);
	for(auto& keyValuePair : m_resources[actorCategoryIndex])
	{
		const std::shared_ptr<Actor> actor = std::dynamic_pointer_cast<Actor>(keyValuePair.second);
		if(actor != nullptr)
		{
			actors.push_back(actor);
		}
		else
		{
			std::cerr << "warning: at NamedResourceStorage::getActors(), non-Actor detected" << std::endl;
		}
	}

	return actors;

	/*std::vector<std::shared_ptr<Actor>> actors;

	for(auto& keyValuePair : m_actorModels)
	{
		actors.push_back(keyValuePair.second);
	}
		
	for(auto& keyValuePair : m_actorLights)
	{
		actors.push_back(keyValuePair.second);
	}

	return actors;*/
}

//std::vector<std::unique_ptr<Actor>> NamedResourceCache::claimAllActors()
//{
//	std::vector<std::unique_ptr<Actor>> actors;
//
//	for(auto& keyValuePair : m_actorModels)
//	{
//		actors.push_back(std::move(keyValuePair.second));
//	}
//
//	for(auto& keyValuePair : m_actorLights)
//	{
//		actors.push_back(std::move(keyValuePair.second));
//	}
//
//	m_actorModels.clear();
//	m_actorLights.clear();
//
//	return actors;
//}

void NamedResourceStorage::reportResourceNotFound(const std::string& categoryName, const std::string& name, const DataTreatment& treatment)
{
	const std::string& message = treatment.notFoundInfo;

	switch(treatment.importance)
	{
	case EDataImportance::OPTIONAL:
		if(!message.empty())
		{
			std::cerr << "warning: optional resource category<" << categoryName << "> name<" << name << "> not found (" << message << ")" << std::endl;
		}
		break;

	case EDataImportance::REQUIRED:
		std::cerr << "warning: required resource category<" << categoryName << "> name<" << name << "> not found";
		if(!message.empty())
		{
			std::cerr << " (" << message << ")";
		}
		std::cerr << std::endl;
		break;
	}
}

bool NamedResourceStorage::checkCategoryIndex(const std::size_t index)
{
	if(index >= (static_cast<std::size_t>(ETypeCategory::MAX) + 1))
	{
		std::cerr << "warning: at NamedResourceStorage::checkCategoryIndex(), category index overflow" << std::endl;
		return false;
	}

	return true;
}

}// end namespace ph