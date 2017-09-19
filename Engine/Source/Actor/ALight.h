#pragma once

#include "Math/Transform/StaticTransform.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Actor/PhysicalActor.h"
#include "FileIO/SDL/TCommandInterface.h"

#include <memory>
#include <vector>

namespace ph
{

class Geometry;
class Material;
class TextureMapper;
class LightSource;
class Transform;

class ALight final : public PhysicalActor, public TCommandInterface<ALight>
{
public:
	ALight();
	ALight(const std::shared_ptr<LightSource>& lightSource);
	ALight(const ALight& other);
	virtual ~ALight() override;

	virtual CookedUnit cook(CookingContext& context) const override;

	const Geometry*    getGeometry() const;
	const Material*    getMaterial() const;
	const LightSource* getLightSource() const;
	void setGeometry(const std::shared_ptr<Geometry>& geometry);
	void setMaterial(const std::shared_ptr<Material>& material);
	void setLightSource(const std::shared_ptr<LightSource>& lightSource);

	ALight& operator = (ALight rhs);

	friend void swap(ALight& first, ALight& second);

private:
	std::shared_ptr<Geometry>    m_geometry;
	std::shared_ptr<Material>    m_material;
	std::shared_ptr<LightSource> m_lightSource;

	CookedUnit buildGeometricLight(CookingContext& context) const;

	// Tries to return a geometry suitable for emitter calculations (can be the 
	// original one if it is already suitable). If the current actor has undesired 
	// configurations, nullptr is returned.
	std::shared_ptr<Geometry> getSanifiedEmitterGeometry(
		CookingContext& context,
		std::unique_ptr<Transform>* out_baseLW, 
		std::unique_ptr<Transform>* out_baseWL) const;

// command interface
public:
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
	static std::unique_ptr<ALight> ciLoad(const InputPacket& packet);
};

}// end namespace ph