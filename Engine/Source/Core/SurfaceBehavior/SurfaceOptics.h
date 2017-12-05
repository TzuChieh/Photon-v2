#pragma once

#include "Common/primitive_type.h"
#include "Math/TVector3.h"
#include "Core/SurfaceBehavior/ESurfacePhenomenon.h"
#include "Core/Quantity/SpectralStrength.h"
#include "Core/HitDetail.h"

namespace ph
{

class Ray;
class BsdfEvaluation;
class BsdfSample;
class BsdfPdfQuery;

class SurfaceOptics
{
public:
	virtual ~SurfaceOptics() = 0;

	void evalBsdf(BsdfEvaluation& eval) const;
	void genBsdfSample(BsdfSample& sample) const;
	void calcBsdfSamplePdf(BsdfPdfQuery& pdfQuery) const;

private:
	virtual void evalBsdf(
		const HitDetail& X, const Vector3R& L, const Vector3R& V,
		SpectralStrength* out_bsdf, 
		ESurfacePhenomenon* out_type) const = 0;

	virtual void genBsdfSample(
		const HitDetail& X, const Vector3R& V,
		Vector3R* out_L, 
		SpectralStrength* out_pdfAppliedBsdf, 
		ESurfacePhenomenon* out_type) const = 0;

	virtual void calcBsdfSamplePdf(
		const HitDetail& X, const Vector3R& L, const Vector3R& V,
		const ESurfacePhenomenon& type, 
		real* out_pdfW) const = 0;
};

}// end namespace ph