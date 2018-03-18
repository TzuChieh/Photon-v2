#pragma once

#include "Common/primitive_type.h"
#include "Math/math_fwd.h"

namespace ph
{

class Intersector;
class LightSampler;
class HitProbe;
class HitDetail;
class DirectLightSample;
class Ray;
class Emitter;
class Primitive;

class Scene final
{
public:
	Scene();
	Scene(const Intersector* intersector, const LightSampler* lightSampler);

	bool isIntersecting(const Ray& ray) const;
	bool isIntersecting(const Ray& ray, HitProbe* out_probe) const;

	const Emitter* pickEmitter(real* const out_PDF) const;
	void genDirectSample(DirectLightSample& sample) const;
	real calcDirectPdfW(const Vector3R& targetPos, const Vector3R& emitPos, const Vector3R& emitN, const Emitter* hitEmitter, const Primitive* hitPrim) const;

private:
	const Intersector*  m_intersector;
	const LightSampler* m_lightSampler;
};

}// end namespace ph