#pragma once

#include "Common/primitive_type.h"
#include "Math/TVector3.h"
#include "Core/SurfaceBehavior/ESurfacePhenomenon.h"

namespace ph
{

class Intersection;
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
	virtual void evaluate(const Intersection& X, const Vector3R& L, const Vector3R& V, 
	                      Vector3R* const out_bsdf, ESurfacePhenomenon* const out_type) const = 0;
	virtual void genSample(const Intersection& X, const Vector3R& V, 
	                       Vector3R* const out_L, Vector3R* const out_pdfAppliedBsdf, ESurfacePhenomenon* const out_type) const = 0;
	virtual void calcSampleDirPdfW(const Intersection& X, const Vector3R& L, const Vector3R& V, const ESurfacePhenomenon& type, 
	                               real* const out_pdfW) const = 0;
};

}// end namespace ph