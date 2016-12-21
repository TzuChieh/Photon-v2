#pragma once

#include "Emitter/Emitter.h"

#include <memory>

namespace ph
{

class Geometry;

class SurfaceEmitter final : public Emitter
{
public:
	SurfaceEmitter(const std::shared_ptr<Geometry>& geometry);
	virtual ~SurfaceEmitter() override;

	virtual void samplePosition(const Vector3f& position, const Vector3f& direction, float32* const out_PDF, Vector3f* const out_emittedRadiance) const override;

private:
	std::shared_ptr<Geometry> m_geometry;
};

}// end namespace ph