#pragma once

#include "Math/TransformInfo.h"
#include "Math/Transform.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Actor/AModel.h"
#include "Actor/Actor.h"

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

class ALight final : public Actor
{
public:
	ALight();
	ALight(const std::shared_ptr<LightSource>& lightSource);
	ALight(const ALight& other);
	~ALight();

	virtual void genCoreActor(CoreActor* const out_coreActor) const override;

	void setLightSource(const std::shared_ptr<LightSource>& lightSource);
	AModel& getModel();

	friend void swap(ALight& first, ALight& second);
	ALight& operator = (ALight rhs);

private:
	// resources
	AModel m_model;
	std::shared_ptr<LightSource> m_lightSource;
};

}// end namespace ph