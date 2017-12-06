#pragma once

#include "Core/Emitter/Emitter.h"
#include "Common/primitive_type.h"
#include "Core/Texture/Texture.h"

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

	virtual void evalEmittedRadiance(const HitDetail& X, SpectralStrength* out_radiance) const override;
	virtual void genDirectSample(const Vector3R& targetPos, Vector3R* out_emitPos, SpectralStrength* out_emittedRadiance, real* out_PDF) const override;
	virtual void genDirectSample(DirectLightSample& sample) const override;
	virtual void genSensingRay(Ray* out_ray, SpectralStrength* out_Le, Vector3R* out_eN, real* out_pdfA, real* out_pdfW) const override;
	virtual real calcDirectSamplePdfW(const Vector3R& targetPos, const Vector3R& emitPos, const Vector3R& emitN, const Primitive* hitPrim) const override;

	void setEmittedRadiance(const std::shared_ptr<Texture>& emittedRadiance);

private:
	std::shared_ptr<Texture> m_emittedRadiance;
	std::vector<const Primitive*> m_primitives;
	real m_reciExtendedArea;
};

}// end namespace ph