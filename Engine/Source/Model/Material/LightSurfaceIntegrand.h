#pragma once

#include "Model/Material/SurfaceIntegrand.h"
#include "Math/Vector3f.h"

namespace ph
{

class LightSurfaceIntegrand : public SurfaceIntegrand
{
public:
	LightSurfaceIntegrand();
	virtual ~LightSurfaceIntegrand() override;

	virtual void genUniformRandomLOverRegion(const Vector3f& N, Vector3f* out_L) const override;
	virtual bool sampleLiWeight(const Vector3f& L, const Vector3f& V, const Vector3f& N, Ray& ray) const override;
	virtual void sampleBRDF(const Intersection& intersection, const Vector3f& L, const Vector3f& V, Vector3f* const out_BRDF) const override;

	virtual inline bool isEmissive() const override
	{
		return true;
	}

	virtual inline void sampleEmittedRadiance(const Intersection& intersection, const Vector3f& L, const Vector3f& V, 
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