#pragma once

#include "Core/Emitter/Emitter.h"
#include "Common/primitive_type.h"
#include "Core/Texture/TTexture.h"
#include "Core/Quantity/SpectralStrength.h"

#include <memory>
#include <vector>

namespace ph
{

class Primitive;

class PrimitiveAreaEmitter final : public Emitter
{
public:
	PrimitiveAreaEmitter(const Primitive* primitive);
	virtual ~PrimitiveAreaEmitter() override;

	virtual void evalEmittedRadiance(const SurfaceHit& X, SpectralStrength* out_radiance) const override;
	virtual void genDirectSample(DirectLightSample& sample) const override;
	virtual void genSensingRay(Ray* out_ray, SpectralStrength* out_Le, Vector3R* out_eN, real* out_pdfA, real* out_pdfW) const override;
	virtual real calcDirectSamplePdfW(const Vector3R& targetPos, const Vector3R& emitPos, const Vector3R& emitN, const Primitive* hitPrim) const override;

	void setEmittedRadiance(const std::shared_ptr<TTexture<SpectralStrength>>& emittedRadiance);
	const Primitive* getPrimitive() const;

private:
	std::shared_ptr<TTexture<SpectralStrength>> m_emittedRadiance;
	const Primitive* m_primitive;
	real m_reciExtendedArea;
};

}// end namespace ph