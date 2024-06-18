#include "Core/Estimator/BVPTDLEstimator.h"
#include "Core/Ray.h"
#include "Core/HitDetail.h"
#include "Core/SurfaceHit.h"
#include "Core/Intersection/PrimitiveMetadata.h"
#include "Core/Intersection/Primitive.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Core/SurfaceBehavior/SurfaceOptics.h"
#include "Core/Emitter/Emitter.h"
#include "Core/SurfaceBehavior/BsdfSampleQuery.h"
#include "Math/Color/Spectrum.h"
#include "Core/LTA/SurfaceTracer.h"
#include "Math/TVector3.h"
#include "Core/Estimator/Integrand.h"

#include <limits>

namespace ph
{

void BVPTDLEstimator::estimate(
	const Ray&        ray,
	const Integrand&  integrand,
	SampleFlow&       sampleFlow,
	EnergyEstimation& out_estimation) const
{
	const lta::SurfaceTracer surfaceTracer{&(integrand.getScene())};

	math::Spectrum& accuRadiance = out_estimation[m_estimationIndex].setColorValues(0);
	math::Spectrum  accuPathWeight(1);
	
	// 0-bounce
	Ray        firstRay;
	SurfaceHit firstHit;
	{
		// backward tracing to light
		firstRay = Ray(ray).reverse();
		firstRay.setRange(0, std::numeric_limits<real>::max());

		if(!surfaceTracer.traceNextSurface(firstRay, BsdfQueryContext{}.sidedness, &firstHit))
		{
			return;
		}

		const auto* const      metadata        = firstHit.getDetail().getPrimitive()->getMetadata();
		const SurfaceBehavior& surfaceBehavior = metadata->getSurface();

		if(surfaceBehavior.getEmitter())
		{
			math::Spectrum emittedRadiance;
			surfaceBehavior.getEmitter()->evalEmittedRadiance(firstHit, &emittedRadiance);

			// Avoid excessive, negative weight and possible NaNs
			emittedRadiance.clampLocal(0.0_r, 1000000000.0_r);

			accuRadiance.addLocal(emittedRadiance.mul(accuPathWeight));
		}
	}

	// 1-bounce
	Ray        secondRay;
	SurfaceHit secondHit;
	{
		const math::Vector3R V = firstRay.getDir().mul(-1.0f);
		const math::Vector3R N = firstHit.getShadingNormal();

		BsdfSampleQuery bsdfSample;
		bsdfSample.inputs.set(firstHit, V);
		if(!surfaceTracer.doBsdfSample(bsdfSample, sampleFlow, &secondRay))
		{
			return;
		}

		if(!surfaceTracer.traceNextSurfaceFrom(
			firstHit, secondRay, BsdfQueryContext{}.sidedness, &secondHit))
		{
			return;
		}

		accuPathWeight.mulLocal(bsdfSample.outputs.getPdfAppliedBsdfCos());

		const auto* const      metadata        = secondHit.getDetail().getPrimitive()->getMetadata();
		const SurfaceBehavior& surfaceBehavior = metadata->getSurface();

		if(surfaceBehavior.getEmitter())
		{
			math::Spectrum emittedRadiance;
			surfaceBehavior.getEmitter()->evalEmittedRadiance(secondHit, &emittedRadiance);

			// avoid excessive, negative weight and possible NaNs
			emittedRadiance.clampLocal(0.0_r, 1000000000.0_r);

			accuRadiance.addLocal(emittedRadiance.mul(accuPathWeight));
		}
	}
}

}// end namespace ph
