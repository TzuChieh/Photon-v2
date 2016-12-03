#pragma once

#include "Model/Material/Integrand/SurfaceIntegrand.h"
#include "Image/Texture.h"

#include <memory>

namespace ph
{

class SiTranslucentMicrofacet : public SurfaceIntegrand
{
public:
	SiTranslucentMicrofacet();
	virtual ~SiTranslucentMicrofacet() override;

	virtual void genUniformRandomV(const Intersection& intersection, const Vector3f& L, Vector3f* out_V) const override;
	virtual void genImportanceRandomV(const Intersection& intersection, const Vector3f& L, Vector3f* out_V) const override;
	virtual void evaluateUniformRandomVPDF(const Intersection& intersection, const Vector3f& L, const Vector3f& V, Vector3f* const out_PDF) const override;
	virtual void evaluateImportanceRandomVPDF(const Intersection& intersection, const Vector3f& L, const Vector3f& V, Vector3f* const out_PDF) const override;
	virtual void evaluateLiWeight(const Intersection& intersection, const Vector3f& L, const Vector3f& V, Vector3f* const out_LiWeight) const override;

	virtual void evaluateImportanceSample(const Intersection& intersection, const Ray& ray, SurfaceSample* const out_sample) const override;

	inline void setF0(const std::shared_ptr<Texture>& f0)
	{
		m_F0 = f0;
	}

	inline void setIOR(const std::shared_ptr<Texture>& ior)
	{
		m_IOR = ior;
	}

	inline void setRoughness(const std::shared_ptr<Texture>& roughness)
	{
		m_roughness = roughness;
	}

private:
	std::shared_ptr<Texture> m_F0;
	std::shared_ptr<Texture> m_IOR;
	std::shared_ptr<Texture> m_roughness;

	float32 calcNormalDistributionTerm(const Intersection& intersection, const Vector3f& H) const;
	float32 calcGeometricShadowingTerm(const Intersection& intersection, const Vector3f& L, const Vector3f& V, const Vector3f& H) const;
	Vector3f calcFresnelTerm(const Intersection& intersection, const Vector3f& V, const Vector3f& H) const;
};

}// end namespace ph