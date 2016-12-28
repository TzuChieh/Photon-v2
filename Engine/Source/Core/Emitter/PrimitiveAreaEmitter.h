#pragma once

#include "Core/Emitter/Emitter.h"
#include "Common/primitive_type.h"
#include "Image/Texture.h"

#include <memory>
#include <vector>

namespace ph
{

class Primitive;

class PrimitiveAreaEmitter final : public Emitter
{
public:
	PrimitiveAreaEmitter(const std::vector<const Primitive*>& primitives);
	virtual ~PrimitiveAreaEmitter() override;

	virtual void evalEmittedRadiance(const Intersection& intersection, Vector3f* const out_emitterRadiance) const override;
	virtual void samplePosition(const Vector3f& position, const Vector3f& direction, float32* const out_PDF, Vector3f* const out_emittedRadiance) const override;

	void setEmittedRadiance(const std::shared_ptr<Texture> emittedRadiance);

private:
	std::shared_ptr<Texture> m_emittedRadiance;
	std::vector<const Primitive*> m_primitives;
	float32 m_reciExtendedArea;
};

}// end namespace ph