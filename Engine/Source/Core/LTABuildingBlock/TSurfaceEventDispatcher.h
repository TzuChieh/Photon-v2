#pragma once

#include "Utility/INoncopyable.h"
#include "Math/math_fwd.h"
#include "Core/SurfaceBehavior/bsdf_query_fwd.h"
#include "Core/LTABuildingBlock/SidednessAgreement.h"

namespace ph
{

class Scene;
class Ray;
class SurfaceHit;

template<ESidednessPolicy POLICY>
class TSurfaceEventDispatcher final : public INoncopyable
{
public:
	explicit TSurfaceEventDispatcher(const Scene* scene);

	bool traceNextSurface(
		const Ray&  ray, 
		SurfaceHit* out_X) const;

	bool doBsdfSample(
		const SurfaceHit& X, 
		BsdfSampleQuery&  bsdfSample, 
		Ray*              out_ray) const;

	bool doBsdfEvaluation(
		const SurfaceHit& X,
		BsdfEvalQuery&    bsdfEval) const;

	bool doBsdfPdfQuery(
		const SurfaceHit& X,
		BsdfPdfQuery&     bsdfPdfQuery) const;
	
private:
	const Scene* m_scene;
};

}// end namespace ph

#include "Core/LTABuildingBlock/TSurfaceEventDispatcher.ipp"
