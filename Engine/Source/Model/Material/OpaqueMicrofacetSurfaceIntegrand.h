#pragma once

#include "Model/Material/SurfaceIntegrand.h"

namespace ph
{

class AbradedOpaque;

class OpaqueMicrofacetSurfaceIntegrand : public SurfaceIntegrand
{
public:
	explicit OpaqueMicrofacetSurfaceIntegrand(const AbradedOpaque* const abradedOpaqueMaterial);
	virtual ~OpaqueMicrofacetSurfaceIntegrand() override;

	virtual void genUniformRandomV(const Intersection& intersection, const Vector3f& L, Vector3f* out_V) const override;
	virtual void genImportanceRandomV(const Intersection& intersection, const Vector3f& L, Vector3f* out_V) const override;
	virtual void evaluateUniformRandomVPDF(const Intersection& intersection, const Vector3f& L, const Vector3f& V, Vector3f* const out_PDF) const override;
	virtual void evaluateImportanceRandomVPDF(const Intersection& intersection, const Vector3f& L, const Vector3f& V, Vector3f* const out_PDF) const override;
	virtual void evaluateLiWeight(const Intersection& intersection, const Vector3f& L, const Vector3f& V, Vector3f* const out_LiWeight) const override;

private:
	const AbradedOpaque* m_abradedOpaqueMaterial;

	float32 calcNormalDistributionTerm(const Vector3f& N, const Vector3f& H) const;
	float32 calcGeometricShadowingTerm(const Vector3f& L, const Vector3f& V, const Vector3f& N, const Vector3f& H) const;
	Vector3f calcFresnelTerm(const Vector3f& V, const Vector3f& H) const;
};

}// end namespace ph