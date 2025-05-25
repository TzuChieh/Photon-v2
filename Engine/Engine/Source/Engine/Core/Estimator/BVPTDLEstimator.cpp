#include "Engine/Core/Estimator/BVPTDLEstimator.h"
#include "Engine/Core/Ray.h"
#include "Engine/Core/HitDetail.h"
#include "Engine/Core/SurfaceHit.h"
#include "Engine/Core/Intersection/PrimitiveMetadata.h"
#include "Engine/Core/Intersection/Primitive.h"
#include "Engine/Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Engine/Core/SurfaceBehavior/SurfaceOptics.h"
#include "Engine/Core/Emitter/SurfaceEmitter.h"
#include "Engine/Core/SurfaceBehavior/BsdfSampleQuery.h"
#include "Engine/Math/Color/Spectrum.h"
#include "Engine/Core/LTA/SurfaceTracer.h"
#include "Engine/Math/TVector3.h"
#include "Engine/Core/Estimator/Integrand.h"

#include <limits>

namespace ph
{

void BVPTDLEstimator::update(const Integrand& integrand)
{}

std::string BVPTDLEstimator::toString() const
{
	return "BVPTDL (Backward Vanilla Path Tracing Direct Lighting Estimator)";
}

std::unique_ptr<TIRayEstimator<math::Spectrum>> BVPTDLEstimator::makeCopy() const
{
	return std::make_unique<BVPTDLEstimator>(*this);
}

void BVPTDLEstimator::estimate(
	const Ray&        ray,
	const Integrand&  integrand,
	SampleFlow&       sampleFlow,
	EnergyEstimation& out_estimation)
{
	const lta::SurfaceTracer surfaceTracer{&(integrand.getScene())};

	math::Spectrum& accuRadiance = out_estimation[getPathEnergyIndex()].setColorValues(0);
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

		if(surfaceBehavior.isEmissive())
		{
			math::Spectrum emittedRadiance;
			surfaceBehavior.getEmitter().evalEmittedEnergy(firstHit, &emittedRadiance);

			// Avoid excessive, negative weight and possible NaNs
			emittedRadiance.safeClampLocal(0.0_r, 1e9_r);

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

		if(surfaceBehavior.isEmissive())
		{
			math::Spectrum emittedRadiance;
			surfaceBehavior.getEmitter().evalEmittedEnergy(secondHit, &emittedRadiance);

			// avoid excessive, negative weight and possible NaNs
			emittedRadiance.safeClampLocal(0.0_r, 1e9_r);

			accuRadiance.addLocal(emittedRadiance.mul(accuPathWeight));
		}
	}
}

}// end namespace ph
