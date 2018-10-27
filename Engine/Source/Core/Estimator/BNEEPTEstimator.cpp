#include "Core/Estimator/BNEEPTEstimator.h"
#include "Core/Ray.h"
#include "World/Scene.h"
#include "Math/TVector3.h"
#include "Core/HitDetail.h"
#include "Actor/Material/Material.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Core/SurfaceBehavior/SurfaceOptics.h"
#include "Core/Intersectable/Primitive.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Math/math.h"
#include "Core/Sample/DirectLightSample.h"
#include "Core/SurfaceBehavior/BsdfEvaluation.h"
#include "Core/SurfaceBehavior/BsdfSample.h"
#include "Core/SurfaceBehavior/BsdfPdfQuery.h"
#include "Core/Quantity/SpectralStrength.h"
#include "Common/assertion.h"
#include "Core/Estimator/Utility/TMis.h"
#include "Core/Estimator/Utility/PtDirectLightEstimator.h"
#include "Core/Estimator/Utility/RussianRoulette.h"
#include "Core/Quantity/SpectralStrength.h"

#include <iostream>

#define RAY_DELTA_DIST 0.0001f
#define MAX_RAY_BOUNCES 10000
//#define MAX_RAY_BOUNCES 1

namespace ph
{

BNEEPTEstimator::BNEEPTEstimator() = default;

BNEEPTEstimator::~BNEEPTEstimator() = default;

void BNEEPTEstimator::radianceAlongRay(
	const Ray&        ray,
	const Integrand&  integrand,
	SpectralStrength& out_radiance,
	SurfaceHit&       out_firstHit) const
{
	const Scene&  scene  = integrand.getScene();
	const Camera& camera = integrand.getCamera();
	const auto&   mis    = TMis<EMisStyle::POWER>();

	// common variables
	//
	SpectralStrength accuRadiance(0);
	SpectralStrength accuLiWeight(1);
	HitProbe         hitProbe;
	SurfaceHit       surfaceHit;
	Vector3R         V;

	// reversing the ray for backward tracing
	//
	Ray tracingRay = Ray(ray).reverse();
	tracingRay.setMinT(0.0001_r);// HACK: hard-coded number
	tracingRay.setMaxT(std::numeric_limits<real>::max());

	if(!scene.isIntersecting(tracingRay, &hitProbe))
	{
		out_radiance = accuRadiance;
		return;
	}

	// 0-bounce direct lighting
	//
	{
		surfaceHit = SurfaceHit(tracingRay, hitProbe);

		// sidedness agreement between real geometry and shading normal
		//
		V = tracingRay.getDirection().mul(-1.0_r);
		if(surfaceHit.getGeometryNormal().dot(V) * surfaceHit.getShadingNormal().dot(V) <= 0.0_r)
		{
			out_radiance = accuRadiance;
			return;
		}

		const PrimitiveMetadata* metadata        = surfaceHit.getDetail().getPrimitive()->getMetadata();
		const SurfaceBehavior&   surfaceBehavior = metadata->getSurface();
		if(surfaceBehavior.getEmitter())
		{
			SpectralStrength radianceLi;
			surfaceBehavior.getEmitter()->evalEmittedRadiance(surfaceHit, &radianceLi);
			accuRadiance.addLocal(radianceLi);
		}

		out_firstHit = surfaceHit;
	}

	// ray bouncing around the scene (1 ~ N bounces)
	//
	for(uint32 numBounces = 0; numBounces < MAX_RAY_BOUNCES; numBounces++)
	{
		// direct light sample
		{
			Vector3R         L;
			real             directPdfW;
			SpectralStrength emittedRadiance;
			if(PtDirectLightEstimator::sample(
				scene, surfaceHit, ray.getTime(),
				&L, &directPdfW, &emittedRadiance))
			{
				const PrimitiveMetadata* metadata        = surfaceHit.getDetail().getPrimitive()->getMetadata();
				const SurfaceBehavior&   surfaceBehavior = metadata->getSurface();

				BsdfEvaluation bsdfEval;
				bsdfEval.inputs.set(surfaceHit, L, V);
				surfaceBehavior.getOptics()->calcBsdf(bsdfEval);
				if(bsdfEval.outputs.isGood())
				{
					BsdfPdfQuery bsdfPdfQuery;
					bsdfPdfQuery.inputs.set(bsdfEval);
					surfaceBehavior.getOptics()->calcBsdfSamplePdfW(bsdfPdfQuery);

					const real     bsdfSamplePdfW = bsdfPdfQuery.outputs.sampleDirPdfW;
					const real     misWeighting = mis.weight(directPdfW, bsdfSamplePdfW);
					const Vector3R N = surfaceHit.getShadingNormal();

					SpectralStrength weight;
					weight = bsdfEval.outputs.bsdf.mul(N.absDot(L));
					weight.mulLocal(accuLiWeight).mulLocal(misWeighting / directPdfW);

					// avoid excessive, negative weight and possible NaNs
					//
					rationalClamp(weight);

					accuRadiance.addLocal(emittedRadiance.mul(weight));
				}
			}
		}// end direct light sample

		// BSDF sample + indirect light sample
		//
		{
			const PrimitiveMetadata* metadata = surfaceHit.getDetail().getPrimitive()->getMetadata();
			const SurfaceBehavior*   surfaceBehavior = &(metadata->getSurface());

			BsdfSample bsdfSample;
			bsdfSample.inputs.set(surfaceHit, V);
			surfaceBehavior->getOptics()->calcBsdfSample(bsdfSample);

			const Vector3R N = surfaceHit.getShadingNormal();
			const Vector3R L = bsdfSample.outputs.L;

			// blackness check & sidedness agreement between real geometry and shading normal
			//
			if(!bsdfSample.outputs.isGood() ||
				surfaceHit.getGeometryNormal().dot(L) * surfaceHit.getShadingNormal().dot(L) <= 0.0_r)
			{
				break;
			}

			PH_ASSERT(L.isFinite());

			BsdfPdfQuery bsdfPdfQuery;
			bsdfPdfQuery.inputs.set(bsdfSample);
			surfaceBehavior->getOptics()->calcBsdfSamplePdfW(bsdfPdfQuery);

			const real bsdfSamplePdfW = bsdfPdfQuery.outputs.sampleDirPdfW;

			// TODO: We should break right after bsdf sampling if the sample 
			// is invalid, but current implementation applied pdf on bsdf
			// beforehand, which might hide cases where pdf = 0.
			if(bsdfSamplePdfW == 0.0_r)
			{
				break;
			}

			const Vector3R directLitPos = surfaceHit.getPosition();

			// trace a ray using BSDF's suggestion
			//
			tracingRay.setOrigin(surfaceHit.getPosition());
			tracingRay.setDirection(L);
			if(!scene.isIntersecting(tracingRay, &hitProbe))
			{
				break;
			}
			const SurfaceHit Xe = SurfaceHit(tracingRay, hitProbe);
			if(Xe.getGeometryNormal().dot(L) * Xe.getShadingNormal().dot(L) <= 0.0_r)
			{
				break;
			}

			metadata = Xe.getDetail().getPrimitive()->getMetadata();
			surfaceBehavior = &(metadata->getSurface());

			const Emitter* emitter = surfaceBehavior->getEmitter();
			if(emitter)
			{
				SpectralStrength radianceLe;
				emitter->evalEmittedRadiance(Xe, &radianceLe);
				if(!radianceLe.isZero())
				{
					// TODO: <directLightPdfW> might be 0, should we terminate MIS if one of two 
					// sampling techniques failed?
					const real directLightPdfW = PtDirectLightEstimator::sampleUnoccludedPdfW(
						scene, surfaceHit, Xe, ray.getTime());

					const real misWeighting = mis.weight(bsdfSamplePdfW, directLightPdfW);

					SpectralStrength weight = bsdfSample.outputs.pdfAppliedBsdf.mul(N.absDot(L));
					weight.mulLocal(accuLiWeight).mulLocal(misWeighting);

					// avoid excessive, negative weight and possible NaNs
					//
					rationalClamp(weight);

					accuRadiance.addLocal(radianceLe.mulLocal(weight));
				}
			}

			const SpectralStrength currentLiWeight = bsdfSample.outputs.pdfAppliedBsdf.mul(N.absDot(L));
			accuLiWeight.mulLocal(currentLiWeight);

			if(numBounces >= 3)
			{
				SpectralStrength weightedAccuLiWeight;
				if(RussianRoulette::surviveOnLuminance(
					accuLiWeight, &weightedAccuLiWeight))
				{
					accuLiWeight = weightedAccuLiWeight;
				}
				else
				{
					break;
				}
			}

			// avoid excessive, negative weight and possible NaNs
			//
			rationalClamp(accuLiWeight);

			if(accuLiWeight.isZero())
			{
				break;
			}

			V = tracingRay.getDirection().mul(-1.0_r);
			PH_ASSERT_MSG(V.isFinite(), V.toString());

			surfaceHit = Xe;
		}
	}// end for each bounces

	out_radiance = accuRadiance;
}

void BNEEPTEstimator::rationalClamp(SpectralStrength& value)
{
	// TODO: should negative value be allowed?
	value.clampLocal(0.0_r, 1000000000.0_r);
}

// command interface

BNEEPTEstimator::BNEEPTEstimator(const InputPacket& packet) :
	PathEstimator(packet)
{}

SdlTypeInfo BNEEPTEstimator::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_ESTIMATOR, "bneept");
}

void BNEEPTEstimator::ciRegister(CommandRegister& cmdRegister)
{
	SdlLoader loader;
	loader.setFunc<BNEEPTEstimator>(ciLoad);
	cmdRegister.setLoader(loader);
}

std::unique_ptr<BNEEPTEstimator> BNEEPTEstimator::ciLoad(const InputPacket& packet)
{
	return std::make_unique<BNEEPTEstimator>(packet);
}

}// end namespace ph