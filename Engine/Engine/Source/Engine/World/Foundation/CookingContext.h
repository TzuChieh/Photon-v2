#pragma once

#include "Engine/World/Foundation/TCookedResourceKey.h"
#include "Engine/Math/Geometry/TAABB3D.h"
#include "Engine/World/Foundation/CommonCookingConfig.h"
#include "Engine/World/Foundation/GeometryCookingConfig.h"
#include "Engine/World/Foundation/MaterialCookingConfig.h"
#include "Engine/World/Foundation/MotionCookingConfig.h"

#include <memory>

namespace ph
{

class VisualWorld;
class CookedResourceCollection;
class TransientResourceCache;
class TransientVisualElement;
class Geometry;
class Material;
class MotionSource;
class Actor;
class CookedGeometry;
class CookedMaterial;
class CookedMotion;

/*! @brief Information about the world being cooked.
*/
class CookingContext final
{
	// TODO: remove
	friend class VisualWorld;

public:
	CookingContext(
		CookedResourceCollection* resources,
		TransientResourceCache* cache);

	explicit CookingContext(const VisualWorld* world);

	const CommonCookingConfig& getCommonConfig() const;
	const GeometryCookingConfig& getGeometryConfig() const;
	const MaterialCookingConfig& getMaterialConfig() const;
	const MotionCookingConfig& getMotionConfig() const;

	/*! @brief Make a context using `config` for geometry cooking.
	Other configuration and referenced world storage are preserved.
	*/
	CookingContext withGeometryConfig(GeometryCookingConfig config) const;

	/*! @brief Make a cooked-resource key for `resource` using current config.
	*/
	///@{
	CookedGeometryKey getKey(const Geometry& resource) const;
	CookedGeometryKey getKey(const std::shared_ptr<const Geometry>& resource) const;
	CookedMaterialKey getKey(const Material& resource) const;
	CookedMaterialKey getKey(const std::shared_ptr<const Material>& resource) const;
	CookedMotionKey getKey(const MotionSource& resource) const;
	CookedMotionKey getKey(const std::shared_ptr<const MotionSource>& resource) const;
	///@}

	void setCommonConfig(CommonCookingConfig config);
	void setGeometryConfig(GeometryCookingConfig config);
	void setMaterialConfig(MaterialCookingConfig config);
	void setMotionConfig(MotionCookingConfig config);
	
	CookedResourceCollection& getResources() const;
	TransientResourceCache& getCache() const;

	/*! @brief Bounds actors cooked in the first level.
	The bound is only available after the first level has done cooking.
	*/
	math::AABB3D getRootActorsBound() const;

	/*! @brief Bounds actors from levels finished cooking.
	The bound is updated every time a level has done cooking. Generally this bound only grows as it
	encapsulates all previous levels including the root level.
	*/
	math::AABB3D getLeafActorsBound() const;

	/*!
	@return Pointer to the cooked geometry. `nullptr` if not found.
	*/
	///@{
	const CookedGeometry* getCooked(const Geometry& geometry) const;
	const CookedGeometry* getCooked(const std::shared_ptr<Geometry>& geometry) const;
	///@}

	/*!
	@return Pointer to the cooked material. `nullptr` if not found.
	*/
	///@{
	const CookedMaterial* getCooked(const Material& material) const;
	const CookedMaterial* getCooked(const std::shared_ptr<Material>& material) const;
	///@}

	/*!
	@return Pointer to the cooked motion. `nullptr` if not found.
	*/
	///@{
	const CookedMotion* getCooked(const MotionSource& motion) const;
	const CookedMotion* getCooked(const std::shared_ptr<MotionSource>& motion) const;
	///@}

	/*!
	@return Pointer to the cooked actor. `nullptr` if not found.
	*/
	const TransientVisualElement* getCached(const std::shared_ptr<Actor>& actor) const;

private:
	const VisualWorld& getWorld() const;

	CommonCookingConfig m_commonConfig;
	GeometryCookingConfig m_geometryConfig;
	MaterialCookingConfig m_materialConfig;
	MotionCookingConfig m_motionConfig;
	const VisualWorld* m_world;
	CookedResourceCollection* m_resources;
	TransientResourceCache* m_cache;
};

}// end namespace ph
