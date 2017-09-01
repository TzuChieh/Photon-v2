#pragma once

#include "Common/primitive_type.h"
#include "Math/TVector3.h"
#include "Core/SurfaceBehavior/ESurfacePhenomenon.h"
#include "Core/Quantity/SpectralStrength.h"
#include "Core/Intersection.h"

namespace ph
{

class Ray;
class BsdfEvaluation;
class BsdfSample;
class BsdfPdfQuery;

class BSDF
{
public:
	virtual ~BSDF() = 0;

	void evaluate(BsdfEvaluation& eval) const;
	void sample(BsdfSample& sample) const;
	void calcPdf(BsdfPdfQuery& pdfQuery) const;

private:
	virtual void evaluate(
		const Intersection& X, const Vector3R& L, const Vector3R& V, 
		SpectralStrength* out_bsdf, 
		ESurfacePhenomenon* out_type) const = 0;

	virtual void genSample(
		const Intersection& X, const Vector3R& V, 
		Vector3R* out_L, 
		SpectralStrength* out_pdfAppliedBsdf, 
		ESurfacePhenomenon* out_type) const = 0;

	virtual void calcSampleDirPdfW(
		const Intersection& X, const Vector3R& L, const Vector3R& V, 
		const ESurfacePhenomenon& type, 
		real* out_pdfW) const = 0;
};

}// end namespace ph