#include "Core/Integrator/BNEEPTIntegrator.h"
#include "Core/Ray.h"
#include "World/Scene.h"
#include "Math/TVector3.h"
#include "Core/HitDetail.h"
#include "Actor/Material/Material.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Core/SurfaceBehavior/SurfaceOptics.h"
#include "Core/Intersectable/Primitive.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Math/Math.h"
#include "Core/Sample/DirectLightSample.h"
#include "Core/SurfaceBehavior/BsdfEvaluation.h"
#include "Core/SurfaceBehavior/BsdfSample.h"
#include "Core/SurfaceBehavior/BsdfPdfQuery.h"
#include "Core/Quantity/SpectralStrength.h"
#include "Common/assertion.h"
#include "Core/Integrator/Utility/TMis.h"
#include "Core/Integrator/Utility/PtDirectLightEstimator.h"
#include "Core/Integrator/Utility/RussianRoulette.h"

#include <iostream>

#define RAY_DELTA_DIST 0.0001f
#define MAX_RAY_BOUNCES 10000
//#define MAX_RAY_BOUNCES 1

namespace ph
{

BNEEPTIntegrator::~BNEEPTIntegrator() = default;

void BNEEPTIntegrator::update(const Scene& scene)
{
	// update nothing
}

void BNEEPTIntegrator::radianceAlongRay(const Ray& ray, const RenderWork& data, std::vector<SenseEvent>& out_senseEvents) const
{
	const Scene&  scene  = *data.scene;
	const Camera& camera = *data.camera;
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
		out_senseEvents.push_back(SenseEvent(accuRadiance));
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
			out_senseEvents.push_back(SenseEvent(accuRadiance));
			return;
		}

		const PrimitiveMetadata* metadata        = surfaceHit.getDetail().getPrimitive()->getMetadata();
		const SurfaceBehavior&   surfaceBehavior = metadata->surfaceBehavior;
		if(surfaceBehavior.getEmitter())
		{
			SpectralStrength radianceLi;
			surfaceBehavior.getEmitter()->evalEmittedRadiance(surfaceHit, &radianceLi);
			accuRadiance.addLocal(radianceLi);
		}
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
				const SurfaceBehavior&   surfaceBehavior = metadata->surfaceBehavior;

				BsdfEvaluation bsdfEval;
				bsdfEval.inputs.set(surfaceHit, L, V);
				surfaceBehavior.getOptics()->evalBsdf(bsdfEval);
				if(bsdfEval.outputs.isGood())
				{
					BsdfPdfQuery bsdfPdfQuery;
					bsdfPdfQuery.inputs.set(bsdfEval);
					surfaceBehavior.getOptics()->calcBsdfSamplePdf(bsdfPdfQuery);

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
			const PrimitiveMetadata* metadata        = surfaceHit.getDetail().getPrimitive()->getMetadata();
			const SurfaceBehavior*   surfaceBehavior = &(metadata->surfaceBehavior);

			BsdfSample bsdfSample;
			bsdfSample.inputs.set(surfaceHit, V);
			surfaceBehavior->getOptics()->genBsdfSample(bsdfSample);

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
			surfaceBehavior->getOptics()->calcBsdfSamplePdf(bsdfPdfQuery);

			const real     bsdfSamplePdfW = bsdfPdfQuery.outputs.sampleDirPdfW;
			const Vector3R directLitPos   = surfaceHit.getPosition();

			// trace a ray using BSDF's suggestion
			//
			tracingRay.setOrigin(surfaceHit.getPosition());
			tracingRay.setDirection(L);
			if(!scene.isIntersecting(tracingRay, &hitProbe))
			{
				break;
			}
			const SurfaceHit Xe = SurfaceHit(tracingRay, hitProbe);

			metadata        = Xe.getDetail().getPrimitive()->getMetadata();
			surfaceBehavior = &(metadata->surfaceBehavior);

			const Emitter* emitter = metadata->surfaceBehavior.getEmitter();
			if(emitter)
			{
				SpectralStrength radianceLe;
				metadata->surfaceBehavior.getEmitter()->evalEmittedRadiance(Xe, &radianceLe);

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

			SpectralStrength currentLiWeight = bsdfSample.outputs.pdfAppliedBsdf.mul(N.absDot(L));
			if(numBounces >= 3)
			{
				SpectralStrength weightedCurrentLiWeight;
				RussianRoulette::surviveOnLuminance(
					currentLiWeight, &weightedCurrentLiWeight);

				currentLiWeight = weightedCurrentLiWeight;
			}
			accuLiWeight.mulLocal(currentLiWeight);

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

	out_senseEvents.push_back(SenseEvent(accuRadiance));
}

void BNEEPTIntegrator::rationalClamp(SpectralStrength& value)
{
	// TODO: should negative value be allowed?
	value.clampLocal(0.0_r, 1000000000.0_r);
}

// command interface

BNEEPTIntegrator::BNEEPTIntegrator(const InputPacket& packet) :
	Integrator(packet)
{}

SdlTypeInfo BNEEPTIntegrator::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_INTEGRATOR, "bneept");
}

void BNEEPTIntegrator::ciRegister(CommandRegister& cmdRegister)
{
	SdlLoader loader;
	loader.setFunc<BNEEPTIntegrator>(ciLoad);
	cmdRegister.setLoader(loader);
}

std::unique_ptr<BNEEPTIntegrator> BNEEPTIntegrator::ciLoad(const InputPacket& packet)
{
	return std::make_unique<BNEEPTIntegrator>(packet);
}

}// end namespace ph