#include "Core/Estimator/Utility/PtDirectLightEstimator.h"
#include "Math/TVector3.h"
#include "World/Scene.h"
#include "Core/Sample/DirectLightSample.h"
#include "Core/SurfaceHit.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Core/HitDetail.h"
#include "Core/Intersectable/Primitive.h"
#include "Core/SurfaceBehavior/BsdfEvaluation.h"
#include "Core/SurfaceBehavior/BsdfPdfQuery.h"
#include "Core/SurfaceBehavior/BsdfSample.h"
#include "Common/assertion.h"

// TODO: better size; without macro
#define RAY_DELTA_DIST 0.0001_r

namespace ph
{

bool PtDirectLightEstimator::sample(
	const Scene&            scene,
	const SurfaceHit&       targetPos,
	const Time&             time,
	Vector3R* const         out_L,
	real* const             out_pdfW,
	SpectralStrength* const out_emittedRadiance)
{
	DirectLightSample directLightSample;
	directLightSample.setDirectSample(targetPos.getPosition());
	scene.genDirectSample(directLightSample);
	if(directLightSample.isDirectSampleGood())
	{
		const Vector3R& toLightVec = directLightSample.emitPos.sub(directLightSample.targetPos);

		// sidedness agreement between real geometry and shading  normal
		//
		if(toLightVec.lengthSquared() > RAY_DELTA_DIST * RAY_DELTA_DIST * 3 &&
		   targetPos.getGeometryNormal().dot(toLightVec) * targetPos.getShadingNormal().dot(toLightVec) > 0.0_r)
		{
			const Ray visRay(targetPos.getPosition(), toLightVec.normalize(), RAY_DELTA_DIST, toLightVec.length() - RAY_DELTA_DIST * 2, time);
			if(!scene.isIntersecting(visRay))
			{
				PH_ASSERT(out_L && out_pdfW && out_emittedRadiance);

				*out_L               = visRay.getDirection();
				*out_pdfW            = directLightSample.pdfW;
				*out_emittedRadiance = directLightSample.radianceLe;

				return true;
			}
		}
	}

	return false;
}

real PtDirectLightEstimator::sampleUnoccludedPdfW(
	const Scene&      scene,
	const SurfaceHit& X,
	const SurfaceHit& Xe,
	const Time&       time)
{
	const Primitive* const emissivePrimitive = Xe.getDetail().getPrimitive();
	const Emitter* const   emitter           = emissivePrimitive->getMetadata()->getSurface().getEmitter();
	PH_ASSERT(emitter);

	return scene.calcDirectPdfW(Xe, X.getPosition());
}

}// end namespace ph