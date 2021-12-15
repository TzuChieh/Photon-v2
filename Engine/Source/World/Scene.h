#pragma once

#include "Common/primitive_type.h"
#include "Math/math_fwd.h"
#include "Common/assertion.h"
#include "Math/Color/Spectrum.h"

namespace ph
{

class Intersector;
class EmitterSampler;
class HitProbe;
class DirectLightSample;
class Ray;
class Emitter;
class Primitive;
class SurfaceHit;
class SampleFlow;

class Scene final
{
public:
	Scene();
	Scene(const Intersector* intersector, const EmitterSampler* emitterSampler);

	bool isOccluding(const Ray& ray) const;
	bool isIntersecting(const Ray& ray, HitProbe* out_probe) const;

	const Emitter* pickEmitter(SampleFlow& sampleFlow, real* const out_PDF) const;
	void genDirectSample(SampleFlow& sampleFlow, DirectLightSample& sample) const;
	real calcDirectPdfW(const SurfaceHit& emitPos, const math::Vector3R& targetPos) const;

	void emitRay(SampleFlow& sampleFlow, Ray* out_ray, math::Spectrum* out_Le, math::Vector3R* out_eN, real* out_pdfA, real* out_pdfW) const;

	void setBackgroundPrimitive(const Primitive* const primitive);

private:
	const Intersector*    m_intersector;
	const EmitterSampler* m_emitterSampler;
	const Primitive*      m_backgroundPrimitive;
};

// In-header Implementations:

inline void Scene::setBackgroundPrimitive(const Primitive* const primitive)
{
	m_backgroundPrimitive = primitive;
}

}// end namespace ph
