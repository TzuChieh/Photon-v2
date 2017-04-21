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

class ALight final : public PhysicalActor, public TCommandInterface<ALight>
{
public:
	ALight();
	ALight(const std::shared_ptr<LightSource>& lightSource);
	ALight(const ALight& other);
	virtual ~ALight() override;

	virtual void cook(CookedActor* const out_cookedActor) const override;

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

// command interface
public:
	ALight(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static ExitStatus ciExecute(const std::shared_ptr<ALight>& targetResource, const std::string& functionName, const InputPacket& packet);
};

}// end namespace ph