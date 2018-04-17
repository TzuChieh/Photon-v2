#pragma once

#include "Common/primitive_type.h"
#include "Math/math_fwd.h"
#include "Math/Transform/Transform.h"
#include "Core/Quantity/SpectralStrength.h"
#include "Core/Emitter/Emitter.h"
#include "Core/Texture/TTexture.h"
#include "Common/assertion.h"

#include <memory>

namespace ph
{

class Primitive;
class SurfaceHit;
class DirectLightSample;
class Ray;
class Time;

class SurfaceEmitter : public Emitter
{
public:
	SurfaceEmitter();
	SurfaceEmitter(const std::shared_ptr<TTexture<SpectralStrength>>& emittedRadiance);
	virtual ~SurfaceEmitter() override;

	virtual void evalEmittedRadiance(const SurfaceHit& X, SpectralStrength* out_radiance) const = 0;
	virtual void genDirectSample(DirectLightSample& sample) const = 0;

	// FIXME: ray time
	virtual void genSensingRay(Ray* out_ray, SpectralStrength* out_Le, Vector3R* out_eN, real* out_pdfA, real* out_pdfW) const = 0;

	virtual real calcDirectSamplePdfW(const Vector3R& targetPos, const Vector3R& emitPos, const Vector3R& emitN, const Primitive* hitPrim) const = 0;

	virtual void setEmittedRadiance(const std::shared_ptr<TTexture<SpectralStrength>>& emittedRadiance);

	inline const TTexture<SpectralStrength>& getEmittedRadiance() const
	{
		PH_ASSERT(m_emittedRadiance != nullptr);

		return *m_emittedRadiance;
	}

	virtual void setFrontFaceEmit();
	virtual void setBackFaceEmit();

protected:
	bool m_isBackFaceEmission;

	bool canEmit(const Vector3R& emitDirection, const Vector3R& N) const;

private:
	std::shared_ptr<TTexture<SpectralStrength>> m_emittedRadiance;
};

}// end namespace ph