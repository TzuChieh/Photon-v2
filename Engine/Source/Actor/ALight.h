#pragma once

#include "Math/TransformInfo.h"
#include "Math/Transform.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Actor/PhysicalActor.h"

#include <memory>
#include <vector>

namespace ph
{

class Geometry;
class Material;
class TextureMapper;
class CookedModelStorage;
class CookedLightStorage;
class LightSource;

class ALight final : public PhysicalActor
{
public:
	ALight();
	ALight(const std::shared_ptr<LightSource>& lightSource);
	ALight(const ALight& other);
	ALight(const InputPacket& packet);
	~ALight();

	virtual void genCoreActor(CoreActor* const out_coreActor) const override;

	const Geometry*    getGeometry() const;
	const Material*    getMaterial() const;
	const LightSource* getLightSource() const;
	void setGeometry(const std::shared_ptr<Geometry>& geometry);
	void setMaterial(const std::shared_ptr<Material>& material);
	void setLightSource(const std::shared_ptr<LightSource>& lightSource);

	friend void swap(ALight& first, ALight& second);
	ALight& operator = (ALight rhs);

private:
	std::shared_ptr<Geometry>    m_geometry;
	std::shared_ptr<Material>    m_material;
	std::shared_ptr<LightSource> m_lightSource;
};

}// end namespace ph