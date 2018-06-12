#include "Core/Integrator/BVPTIntegrator.h"
#include "Core/Ray.h"
#include "Core/HitDetail.h"
#include "Core/SurfaceHit.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Core/SurfaceBehavior/SurfaceOptics.h"
#include "Core/Intersectable/Primitive.h"
#include "Core/Emitter/Emitter.h"
#include "Core/SurfaceBehavior/BsdfSample.h"
#include "Core/Quantity/SpectralStrength.h"
#include "Core/Integrator/Utility/PtVolumetricEstimator.h"
#include "Core/Integrator/Utility/TSurfaceEventDispatcher.h"
#include "Core/Integrator/Utility/RussianRoulette.h"
#include "FileIO/SDL/InputPacket.h"
#include "Math/TVector3.h"

#include <iostream>

#define MAX_RAY_BOUNCES 10000
//#define MAX_RAY_BOUNCES 1

namespace ph
{

BVPTIntegrator::BVPTIntegrator() : 
	AbstractPathIntegrator()
{}

BVPTIntegrator::BVPTIntegrator(const BVPTIntegrator& other) : 
	AbstractPathIntegrator(other)
{}

std::unique_ptr<Integrator> BVPTIntegrator::makeReproduction() const
{
	return std::make_unique<BVPTIntegrator>(*this);
}

void BVPTIntegrator::tracePath(
	const Ray&              ray,
	SpectralStrength* const out_lightEnergy,
	SurfaceHit*       const out_firstHit) const
{
	const auto& surfaceEventDispatcher = TSurfaceEventDispatcher<ESidednessAgreement::STRICT>(m_scene);

	uint32 numBounces = 0;
	SpectralStrength accuRadiance(0);
	SpectralStrength accuLiWeight(1);

	// backward tracing to light
	Ray tracingRay = Ray(ray).reverse();
	tracingRay.setMinT(0.0001_r);// HACK: hard-coded number
	tracingRay.setMaxT(std::numeric_limits<real>::max());

	SurfaceHit surfaceHit;
	while(numBounces <= MAX_RAY_BOUNCES && 
	      surfaceEventDispatcher.traceNextSurface(tracingRay, &surfaceHit))
	{
		const auto* const     metadata            = surfaceHit.getDetail().getPrimitive()->getMetadata();
		const SurfaceBehavior& hitSurfaceBehavior = metadata->getSurface();

		if(hitSurfaceBehavior.getEmitter())
		{
			SpectralStrength radianceLi;
			hitSurfaceBehavior.getEmitter()->evalEmittedRadiance(surfaceHit, &radianceLi);

			// avoid excessive, negative weight and possible NaNs
			accuLiWeight.clampLocal(0.0_r, 1000000000.0_r);

			accuRadiance.addLocal(radianceLi.mul(accuLiWeight));
		}

		const Vector3R V = tracingRay.getDirection().mul(-1.0f);
		const Vector3R N = surfaceHit.getShadingNormal();

		BsdfSample bsdfSample;
		bsdfSample.inputs.set(surfaceHit, V);
		Ray nextRay;
		if(!surfaceEventDispatcher.doBsdfSample(surfaceHit, bsdfSample, &nextRay))
		{
			break;
		}

		const Vector3R L = bsdfSample.outputs.L;

		SpectralStrength liWeight = bsdfSample.outputs.pdfAppliedBsdf.mul(N.absDot(L));
		if(numBounces >= 3)
		{
			SpectralStrength weightedLiWeight;
			RussianRoulette::surviveOnLuminance(liWeight, &weightedLiWeight);

			liWeight = weightedLiWeight;
		}
		accuLiWeight.mulLocal(liWeight);

		if(accuLiWeight.isZero())
		{
			break;
		}

		// volume test
		{
			const PrimitiveMetadata* metadata = surfaceHit.getDetail().getPrimitive()->getMetadata();
			if(surfaceHit.hasInteriorOptics() && surfaceHit.getShadingNormal().dot(V) * surfaceHit.getShadingNormal().dot(L) < 0.0_r)
			{
				SurfaceHit Xe;
				Vector3R endV;
				SpectralStrength weight;
				SpectralStrength radiance;
				PtVolumetricEstimator::sample(*m_scene, surfaceHit, L, &Xe, &endV, &weight, &radiance);

				accuLiWeight.mulLocal(weight);
				if(accuLiWeight.isZero())
				{
					break;
				}

				BsdfSample bsdfSample;
				bsdfSample.inputs.set(Xe, endV);
				metadata->getSurface().getOptics()->genBsdfSample(bsdfSample);
				if(!bsdfSample.outputs.isGood())
				{
					break;
				}

				accuLiWeight.mulLocal(bsdfSample.outputs.pdfAppliedBsdf);
				if(accuLiWeight.isZero())
				{
					break;
				}

				const Vector3R nextRayOrigin(Xe.getPosition());
				const Vector3R nextRayDirection(bsdfSample.outputs.L);
				tracingRay.setOrigin(nextRayOrigin);
				tracingRay.setDirection(nextRayDirection);
			}
			else
			{
				tracingRay = nextRay;
			}
		}
		numBounces++;
	}// end while

	*out_lightEnergy = accuRadiance;
}

BVPTIntegrator& BVPTIntegrator::operator = (BVPTIntegrator rhs)
{
	swap(*this, rhs);

	return *this;
}

void swap(BVPTIntegrator& first, BVPTIntegrator& second)
{
	using std::swap;

	swap(static_cast<AbstractPathIntegrator&>(first), static_cast<AbstractPathIntegrator&>(second));
}

// command interface

BVPTIntegrator::BVPTIntegrator(const InputPacket& packet) :
	AbstractPathIntegrator(packet)
{}

SdlTypeInfo BVPTIntegrator::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_INTEGRATOR, "bvpt");
}

void BVPTIntegrator::ciRegister(CommandRegister& cmdRegister)
{
	SdlLoader loader;
	loader.setFunc<BVPTIntegrator>(ciLoad);
	cmdRegister.setLoader(loader);
}

std::unique_ptr<BVPTIntegrator> BVPTIntegrator::ciLoad(const InputPacket& packet)
{
	return std::make_unique<BVPTIntegrator>(packet);
}

}// end namespace ph