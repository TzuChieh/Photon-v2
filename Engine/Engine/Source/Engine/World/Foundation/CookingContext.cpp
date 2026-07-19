#include "Engine/World/Foundation/CookingContext.h"
#include "Engine/World/Foundation/CookedResourceCollection.h"
#include "Engine/World/VisualWorld.h"
#include "Engine/Actor/Geometry/Geometry.h"
#include "Engine/Actor/Material/Material.h"
#include "Engine/Actor/MotionSource/MotionSource.h"
#include "Engine/Actor/Actor.h"
#include "Engine/World/Foundation/TransientVisualElement.h"

#include <Common/logging.h>
#include <Common/assertion.h>

#include <utility>
#include <type_traits>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(CookingContext, World);

CookingContext::CookingContext(
	CookedResourceCollection* const resources,
	TransientResourceCache* const cache)
	: m_commonConfig()
	, m_geometryConfig()
	, m_materialConfig()
	, m_motionConfig()
	, m_world(nullptr)
	, m_resources(resources)
	, m_cache(cache)
{
	PH_LOG(CookingContext, Note,
		"created context, contains resource storage: {}, contains transient cache: {}",
		m_resources != nullptr, m_cache != nullptr);
}

CookingContext::CookingContext(const VisualWorld* const world)
	: CookingContext(
		world ? world->getCookedResources() : nullptr,
		world ? world->getCache() : nullptr)
{
	m_world = world;
}

const CommonCookingConfig& CookingContext::getCommonConfig() const
{
	return m_commonConfig;
}

const GeometryCookingConfig& CookingContext::getGeometryConfig() const
{
	return m_geometryConfig;
}

const MaterialCookingConfig& CookingContext::getMaterialConfig() const
{
	return m_materialConfig;
}

const MotionCookingConfig& CookingContext::getMotionConfig() const
{
	return m_motionConfig;
}

CookingContext CookingContext::withGeometryConfig(GeometryCookingConfig config) const
{
	CookingContext ctx = *this;
	ctx.setGeometryConfig(std::move(config));
	return ctx;
}

CookedGeometryKey CookingContext::getKey(const Geometry& resource) const
{
	return CookedGeometryKey(
		resource.getId(), getCommonConfig(), getGeometryConfig());
}

CookedGeometryKey CookingContext::getKey(const std::shared_ptr<const Geometry>& resource) const
{
	PH_ASSERT(resource);
	return getKey(*resource);
}

CookedMaterialKey CookingContext::getKey(const Material& resource) const
{
	return CookedMaterialKey(
		resource.getId(), getCommonConfig(), getMaterialConfig());
}

CookedMaterialKey CookingContext::getKey(const std::shared_ptr<const Material>& resource) const
{
	PH_ASSERT(resource);
	return getKey(*resource);
}

CookedMotionKey CookingContext::getKey(const MotionSource& resource) const
{
	return CookedMotionKey(
		resource.getId(), getCommonConfig(), getMotionConfig());
}

CookedMotionKey CookingContext::getKey(
	const std::shared_ptr<const MotionSource>& resource) const
{
	PH_ASSERT(resource);
	return getKey(*resource);
}

void CookingContext::setCommonConfig(CommonCookingConfig config)
{
	m_commonConfig = std::move(config);
}

void CookingContext::setGeometryConfig(GeometryCookingConfig config)
{
	m_geometryConfig = std::move(config);
}

void CookingContext::setMaterialConfig(MaterialCookingConfig config)
{
	m_materialConfig = std::move(config);
}

void CookingContext::setMotionConfig(MotionCookingConfig config)
{
	m_motionConfig = std::move(config);
}

CookedResourceCollection& CookingContext::getResources() const
{
	PH_ASSERT(m_resources);
	return *m_resources;
}

TransientResourceCache& CookingContext::getCache() const
{
	PH_ASSERT(m_cache);
	return *m_cache;
}

math::AABB3D CookingContext::getRootActorsBound() const
{
	return getWorld().getRootActorsBound();
}

math::AABB3D CookingContext::getLeafActorsBound() const
{
	return getWorld().getLeafActorsBound();
}

const CookedGeometry* CookingContext::getCooked(const Geometry& geometry) const
{
	return getResources().getGeometry(getKey(geometry));
}

const CookedGeometry* CookingContext::getCooked(const std::shared_ptr<Geometry>& geometry) const
{
	return geometry ? getCooked(*geometry) : nullptr;
}

const CookedMaterial* CookingContext::getCooked(const Material& material) const
{
	return getResources().getMaterial(getKey(material));
}

const CookedMaterial* CookingContext::getCooked(const std::shared_ptr<Material>& material) const
{
	return material ? getCooked(*material) : nullptr;
}

const CookedMotion* CookingContext::getCooked(const MotionSource& motion) const
{
	return getResources().getMotion(getKey(motion));
}

const CookedMotion* CookingContext::getCooked(const std::shared_ptr<MotionSource>& motion) const
{
	return motion ? getCooked(*motion) : nullptr;
}

const TransientVisualElement* CookingContext::getCached(const std::shared_ptr<Actor>& actor) const
{
	return actor != nullptr
		? getCache().getVisualElement(actor->getId())
		: nullptr;
}

const VisualWorld& CookingContext::getWorld() const
{
	PH_ASSERT(m_world);
	return *m_world;
}

static_assert(std::is_copy_constructible_v<CookingContext>,
	"CookingContext must be copyable so modified context can be easily created.");

}// end namespace ph
