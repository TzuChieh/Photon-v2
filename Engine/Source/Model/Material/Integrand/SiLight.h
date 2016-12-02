#pragma once

#include "Model/Material/Integrand/SurfaceIntegrand.h"
#include "Math/Vector3f.h"

namespace ph
{

class SiLight : public SurfaceIntegrand
{
public:
	SiLight();
	virtual ~SiLight() override;

	virtual void genUniformRandomV(const Intersection& intersection, const Vector3f& L, Vector3f* out_V) const override;
	virtual void genImportanceRandomV(const Intersection& intersection, const Vector3f& L, Vector3f* out_V) const override;
	virtual void evaluateUniformRandomVPDF(const Intersection& intersection, const Vector3f& L, const Vector3f& V, Vector3f* const out_PDF) const override;
	virtual void evaluateImportanceRandomVPDF(const Intersection& intersection, const Vector3f& L, const Vector3f& V, Vector3f* const out_PDF) const override;
	virtual void evaluateLiWeight(const Intersection& intersection, const Vector3f& L, const Vector3f& V, Vector3f* const out_LiWeight) const override;

	virtual inline bool isEmissive() const override
	{
		return true;
	}

	virtual inline void evaluateEmittedRadiance(const Intersection& intersection, const Vector3f& L, const Vector3f& V,
	                                            Vector3f* const out_emittedRadiance) const override
	{
		out_emittedRadiance->set(m_emittedRadiance);
	}

	inline void setEmittedRadiance(const float32 r, const float32 g, const float32 b)
	{
		m_emittedRadiance.set(r, g, b);
	}

private:
	Vector3f m_emittedRadiance;
};

}// end namespace ph