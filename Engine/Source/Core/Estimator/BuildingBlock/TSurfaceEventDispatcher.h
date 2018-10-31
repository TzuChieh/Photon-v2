#pragma once

#include "Utility/INoncopyable.h"
#include "Math/math_fwd.h"
#include "Core/SidednessAgreement.h"

namespace ph
{

class Scene;
class Ray;
class SurfaceHit;
class BsdfSample;
class BsdfEvaluation;
class BsdfPdfQuery;

template<ESaPolicy POLICY>
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
};

}// end namespace ph

#include "Core/Estimator/BuildingBlock/TSurfaceEventDispatcher.ipp"