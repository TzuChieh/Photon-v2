#pragma once

#include "Core/Emitter/SurfaceEmitter.h"
#include "Common/primitive_type.h"

#include <memory>
#include <vector>

namespace ph
{

class Primitive;

class DiffuseSurfaceEmitter final : public SurfaceEmitter
{
public:
	DiffuseSurfaceEmitter(const Primitive* surface);

	void evalEmittedRadiance(const SurfaceHit& X, SpectralStrength* out_radiance) const override;
	void genDirectSample(DirectLightSample& sample) const override;
	void genSensingRay(Ray* out_ray, SpectralStrength* out_Le, Vector3R* out_eN, real* out_pdfA, real* out_pdfW) const override;
	real calcDirectSamplePdfW(const SurfaceHit& emitPos, const Vector3R& targetPos) const override;

	const Primitive* getSurface() const;

private:
	const Primitive* m_surface;
	real m_reciExtendedArea;
};

}// end namespace ph