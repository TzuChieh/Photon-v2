#include "Core/LTABuildingBlock/TDirectLightEstimator.h"
#include "Math/TVector3.h"
#include "World/Scene.h"
#include "Core/Sample/DirectLightSample.h"
#include "Core/SurfaceHit.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Core/HitDetail.h"
#include "Core/Intersectable/Primitive.h"
#include "Common/assertion.h"

#include <algorithm>

namespace ph
{

namespace
{
	// FIXME: hardcoded number
	constexpr real DL_RAY_DELTA_DIST = 0.0001_r;
}

template<ESidednessPolicy POLICY>
inline TDirectLightEstimator<POLICY>::TDirectLightEstimator(const Scene* const scene) : 
	m_scene(scene)
{
	PH_ASSERT(scene);
}

template<ESidednessPolicy POLICY>
inline bool TDirectLightEstimator<POLICY>::sample(
	const SurfaceHit&       targetPos,
	const Time&             time,
	SampleFlow&             sampleFlow,
	math::Vector3R* const   out_L,
	real* const             out_pdfW,
	SpectralStrength* const out_emittedRadiance)
{
	/*const PrimitiveMetadata* metadata = targetPos.getDetail().getPrimitive()->getMetadata();
	const SurfaceOptics* optics = metadata->getSurface().getOptics();
	if(optics->getAllPhenomena().hasAtLeastOne({ESurfacePhenomenon::DELTA_REFLECTION, ESurfacePhenomenon::DELTA_TRANSMISSION}))
	{
		return false;
	}*/

	DirectLightSample directLightSample;
	directLightSample.setDirectSample(targetPos.getPosition());
	m_scene->genDirectSample(sampleFlow, directLightSample);
	if(directLightSample.isDirectSampleGood())
	{
		const math::Vector3R toLightVec = directLightSample.emitPos.sub(directLightSample.targetPos);

		// sidedness agreement between real geometry and shading normal
		//
		if(toLightVec.lengthSquared() > math::squared(DL_RAY_DELTA_DIST * 3) &&
		   SidednessAgreement(POLICY).isSidednessAgreed(targetPos, toLightVec))
		{
			const Ray visRay(targetPos.getPosition(), toLightVec.normalize(), DL_RAY_DELTA_DIST, toLightVec.length() - DL_RAY_DELTA_DIST * 2, time);
			if(!m_scene->isIntersecting(visRay))
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

template<ESidednessPolicy POLICY>
inline real TDirectLightEstimator<POLICY>::samplePdfWUnoccluded(
	const SurfaceHit& X,
	const SurfaceHit& Xe,
	const Time&       time)
{
	// FIXME: redundant pointers
	const Primitive* const emissivePrimitive = Xe.getDetail().getPrimitive();
	const Emitter* const   emitter           = emissivePrimitive->getMetadata()->getSurface().getEmitter();
	PH_ASSERT(emitter);

	return m_scene->calcDirectPdfW(Xe, X.getPosition());
}

}// end namespace ph
