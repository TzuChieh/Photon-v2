#pragma once

#include "Actor/Light/LightSource/LightSource.h"

#include <memory>

namespace ph
{

class Vector3f;
class Texture;

class AreaSource final : public LightSource
{
public:
	AreaSource(const Vector3f& emittedRadiance);
	virtual ~AreaSource() override;

	virtual void buildEmitters(PrimitiveStorage* const out_primitiveStorage,
	                           EmitterStorage* const out_emitterStorage,
	                           const Model& lightModel) const override;

private:
	std::shared_ptr<Texture> m_emittedRadiance;
};

}// end namespace ph