#pragma once

#include "Common/primitive_type.h"
#include "Math/math_fwd.h"
#include "Common/assertion.h"
#include "Core/Quantity/SpectralStrength.h"

namespace ph
{

class Intersector;
class EmitterSampler;
class HitProbe;
class HitDetail;
class DirectLightSample;
class Ray;
class Emitter;
class Primitive;
class SurfaceHit;

class Scene final
{
public:
	Scene();
	Scene(const Intersector* intersector, const EmitterSampler* emitterSampler);

	bool isIntersecting(const Ray& ray) const;
	bool isIntersecting(const Ray& ray, HitProbe* out_probe) const;

	const Emitter* pickEmitter(real* const out_PDF) const;
	void genDirectSample(DirectLightSample& sample) const;
	real calcDirectPdfW(const SurfaceHit& emitPos, const math::Vector3R& targetPos) const;

	void genSensingRay(Ray* out_ray, SpectralStrength* out_Le, math::Vector3R* out_eN, real* out_pdfA, real* out_pdfW) const;

	// HACK
	void setBackgroundEmitterPrimitive(const Primitive* primitive)
	{
		PH_ASSERT(primitive);

		m_backgroundEmitterPrimitive = primitive;
	}

private:
	const Intersector*    m_intersector;
	const EmitterSampler* m_emitterSampler;

	// HACK
	const Primitive* m_backgroundEmitterPrimitive;
};

}// end namespace ph
