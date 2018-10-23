#pragma once

#include "Common/primitive_type.h"
#include "Math/TVector3.h"
#include "Core/SurfaceBehavior/surface_optics_fwd.h"
#include "Core/Quantity/SpectralStrength.h"
#include "Core/SurfaceHit.h"

namespace ph
{

class Ray;
class BsdfEvaluation;
class BsdfSample;
class BsdfPdfQuery;
class SidednessAgreement;

class SurfaceOptics
{
	// FIXME: sort-of hacked, should clarify the need of this in the future
	friend class LerpedSurfaceOptics;

public:
	SurfaceOptics();
	virtual ~SurfaceOptics();

	void calcBsdf(BsdfEvaluation& eval) const;
	void calcBsdfSample(BsdfSample& sample) const;
	void calcBsdfSamplePdfW(BsdfPdfQuery& pdfQuery) const;

	const SurfacePhenomena& getPhenomena() const;
	unsigned int numComponents() const;

protected:
	SurfacePhenomena m_phenomena;
	unsigned int     m_numComponents;

private:
	virtual void calcBsdf(
		const SurfaceHit&         X, 
		const Vector3R&           L, 
		const Vector3R&           V,
		const SidednessAgreement& sidedness,
		SpectralStrength*         out_bsdf) const = 0;

	virtual void calcBsdfSample(
		const SurfaceHit&         X, 
		const Vector3R&           V,
		const SidednessAgreement& sidedness,
		Vector3R*                 out_L, 
		SpectralStrength*         out_pdfAppliedBsdf) const = 0;

	virtual void calcBsdfSamplePdfW(
		const SurfaceHit&         X, 
		const Vector3R&           L, 
		const Vector3R&           V,
		const SidednessAgreement& sidedness,
		real*                     out_pdfW) const = 0;
};

// In-header Implementations:

inline const SurfacePhenomena& SurfaceOptics::getPhenomena() const
{
	return m_phenomena;
}

inline unsigned int SurfaceOptics::numComponents() const
{
	return m_numComponents;
}

}// end namespace ph