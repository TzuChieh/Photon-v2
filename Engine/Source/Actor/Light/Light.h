#pragma once

#include "Math/TransformInfo.h"
#include "Math/Transform.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Actor/Model/Model.h"

#include <memory>
#include <vector>

namespace ph
{

class Geometry;
class Material;
class TextureMapper;
class PrimitiveStorage;
class EmitterStorage;
class LightSource;

class Light final
{
public:
	Light();
	Light(const std::shared_ptr<LightSource>& lightSource);
	Light(const Light& other);
	~Light();

	void cookData(PrimitiveStorage* const out_primitiveStorage, EmitterStorage* const out_emitterStorage) const;
	void setLightSource(const std::shared_ptr<LightSource>& lightSource);
	Model& getModel();

	friend void swap(Light& first, Light& second);
	Light& operator = (Light rhs);

private:
	// resources
	Model m_model;
	std::shared_ptr<LightSource> m_lightSource;
};

}// end namespace ph