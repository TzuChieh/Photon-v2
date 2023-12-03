#include "Core/Estimator/BVPTDLEstimator.h"
#include "Core/Ray.h"
#include "Core/HitDetail.h"
#include "Core/SurfaceHit.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Core/SurfaceBehavior/SurfaceOptics.h"
#include "Core/Intersectable/Primitive.h"
#include "Core/Emitter/Emitter.h"
#include "Core/SurfaceBehavior/BsdfSampleQuery.h"
#include "Math/Color/Spectrum.h"
#include "Core/LTABuildingBlock/SurfaceTracer.h"
#include "Math/TVector3.h"
#include "Core/Estimator/Integrand.h"

#include <iostream>

namespace ph
{

void BVPTDLEstimator::estimate(
	const Ray&        ray,
	const Integrand&  integrand,
	SampleFlow&       sampleFlow,
	EnergyEstimation& out_estimation) const
{
	const SurfaceTracer surfaceTracer(&(integrand.getScene()));

	math::Spectrum& accuRadiance = out_estimation[m_estimationIndex].setColorValues(0);
	math::Spectrum  accuPathWeight(1);
	
	// 0-bounce
	Ray        firstRay;
	SurfaceHit firstHit;
	{
		// backward tracing to light
		firstRay = Ray(ray).reverse();
		firstRay.setMinT(0.0001_r);// HACK: hard-coded number
		firstRay.setMaxT(std::numeric_limits<real>::max());

		if(!surfaceTracer.traceNextSurface(firstRay, BsdfQueryContext().sidedness, &firstHit))
		{
			return;
		}

		const auto* const      metadata        = firstHit.getDetail().getPrimitive()->getMetadata();
		const SurfaceBehavior& surfaceBehavior = metadata->getSurface();

		if(surfaceBehavior.getEmitter())
		{
			math::Spectrum emittedRadiance;
			surfaceBehavior.getEmitter()->evalEmittedRadiance(firstHit, &emittedRadiance);

			// avoid excessive, negative weight and possible NaNs
			emittedRadiance.clampLocal(0.0_r, 1000000000.0_r);

			accuRadiance.addLocal(emittedRadiance.mul(accuPathWeight));
		}
	}

	// 1-bounce
	Ray        secondRay;
	SurfaceHit secondHit;
	{
		const math::Vector3R V = firstRay.getDirection().mul(-1.0f);
		const math::Vector3R N = firstHit.getShadingNormal();

		BsdfSampleQuery bsdfSample;
		bsdfSample.inputs.set(firstHit, V);
		if(!surfaceTracer.doBsdfSample(bsdfSample, sampleFlow, &secondRay))
		{
			return;
		}

		if(!surfaceTracer.traceNextSurface(secondRay, BsdfQueryContext().sidedness, &secondHit))
		{
			return;
		}

		const math::Vector3R L          = bsdfSample.outputs.L;
		const math::Spectrum pathWeight = bsdfSample.outputs.pdfAppliedBsdf.mul(N.absDot(L));

		accuPathWeight.mulLocal(pathWeight);

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
