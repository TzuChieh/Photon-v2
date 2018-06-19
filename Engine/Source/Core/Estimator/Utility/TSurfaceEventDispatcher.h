#pragma once

#include "Utility/INoncopyable.h"
#include "Math/math_fwd.h"

namespace ph
{

class Scene;
class Ray;
class SurfaceHit;
class BsdfSample;
class BsdfEvaluation;
class BsdfPdfQuery;

/*
	Options for dealing with situations where a vector is within the hemisphere
	defined by geometry normal but outside the one defined by shading normal or
	vice versa.
*/
enum class ESidednessAgreement
{
	// Perform any calculations without caring sidedness agreement.
	DO_NOT_CARE,

	// A vector must lies in hemispheres defined by geometry normal and shading
	// normal simultaneously.
	STRICT
};

template<ESidednessAgreement SA>
class TSurfaceEventDispatcher final : public INoncopyable
{
public:
	TSurfaceEventDispatcher(const Scene* scene);

	bool traceNextSurface(
		const Ray&  ray, 
		SurfaceHit* out_X) const;

	bool doBsdfSample(
		const SurfaceHit& X, 
		BsdfSample&       bsdfSample, 
		Ray*              out_ray) const;

	bool doBsdfEvaluation(
		const SurfaceHit& X,
		BsdfEvaluation&   bsdfEvaluation) const;

	bool doBsdfPdfQuery(
		const SurfaceHit& X,
		BsdfPdfQuery&     bsdfPdfQuery) const;
	
private:
	const Scene* m_scene;

	bool isSidednessAgreed(
		const SurfaceHit& X, 
		const Vector3R&   targetVector) const;

	void adjustForSidednessAgreement(
		SurfaceHit& X) const;
};

}// end namespace ph

#include "Core/Estimator/Utility/TSurfaceEventDispatcher.ipp"