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

enum class ESidednessAgreement
{
	DO_NOT_CARE,
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

#include "Core/Integrator/Utility/TSurfaceEventDispatcher.ipp"