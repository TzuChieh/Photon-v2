#pragma once

#include "Core/Renderer/PM/TRadianceEvaluationWork.h"
#include "Math/constant.h"
#include "Core/Intersectable/Primitive.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Core/SurfaceBehavior/SurfaceOptics.h"
#include "Core/SurfaceBehavior/BsdfEvalQuery.h"
#include "Core/SurfaceBehavior/BsdfSampleQuery.h"
#include "Core/SurfaceBehavior/BsdfPdfQuery.h"

#include <Common/assertion.h>

#include <vector>

namespace ph
{

template<CPhoton Photon>
inline TRadianceEvaluationWork<Photon>::TRadianceEvaluationWork(
	const TPhotonMap<Photon>* photonMap,
	const std::size_t         numPhotonPaths) :
	m_photonMap(photonMap),
	m_numPhotonPaths(numPhotonPaths)
{
	PH_ASSERT(photonMap);
	PH_ASSERT_GT(numPhotonPaths, 0);
}

template<CPhoton Photon>
inline void TRadianceEvaluationWork<Photon>::doWork()
{
	/*for(std::size_t i = 0; i < m_numViewpoints; ++i)
	{
		const Viewpoint& viewpoint = m_viewpoints[i];
		
		const Spectrum& radiance = evaluateRadiance(
			viewpoint.hit,
			viewpoint.L,
			viewpoint.radius);

		real filmXPx = viewpoint.filmNdcPos.x * static_cast<real>(m_film->getActualResPx().x);
		real filmYPx = viewpoint.filmNdcPos.y * static_cast<real>(m_film->getActualResPx().y);
		m_film->addSample(filmXPx, filmYPx, radiance);
	}*/
}

template<CPhoton Photon>
inline math::Spectrum TRadianceEvaluationWork<Photon>::evaluateRadiance(
	const SurfaceHit&     location,
	const math::Vector3R& excitant,
	const real            kernelRadius)
{
	/*static_assert(
		Photon::has<EPhotonData::INCIDENT_DIR>() &&
		Photon::has<EPhotonData::RADIANCE>()     &&
		Photon::has<EPhotonData::THROUGHPUT>(),
		"provided Photon do not have sufficient data");

	const real reciKernelArea        = 1.0_r / (kernelRadius * kernelRadius * PH_PI_REAL);
	const real reciNumEmittedPhotons = 1.0_r / static_cast<real>(m_numEmittedPhotons);

	m_photonCache.clear();
	m_photonMap->findWithinRange(location.getPosition(), kernelRadius, m_photonCache);*/

	BsdfEvalQuery  bsdfEval;
	math::Spectrum radiance(0);
	//for(const auto& photon : m_photonCache)
	//{
	//	const Vector3R V  = excitant;
	//	const Vector3R L  = photon.getReference<EPhotonData::INCIDENT_DIR>();
	//	const Vector3R Ng = location.getGeometryNormal();
	//	const Vector3R Ns = location.getShadingNormal();

	//	const PrimitiveMetadata* const metadata = location.getDetail().getPrimitive()->getMetadata();
	//	const SurfaceOptics*     const optics = metadata->getSurface().getOptics();

	//	bsdfEval.inputs.set(location, L, V, ALL_ELEMENTALS, ETransport::RADIANCE);
	//	optics->calcBsdf(bsdfEval);

	//	if(!bsdfEval.outputs.isGood())
	//	{
	//		continue;
	//	}

	//	Spectrum throughput(1.0_r);
	//	throughput.mulLocal(bsdfEval.outputs.bsdf);
	//	//throughput.mulLocal(Ns.absDot(V) * Ng.absDot(L) / Ng.absDot(V) / Ns.absDot(L));
	//	throughput.mulLocal(photon.getReference<EPhotonData::THROUGHPUT>());

	//	radiance.addLocal(throughput * photon.getReference<EPhotonData::RADIANCE>() * reciKernelArea * reciNumEmittedPhotons);
	//}

	/*for(std::size_t i = 0; i < radiance.NUM_VALUES; ++i)
	{
		PH_ASSERT_MSG(!std::isinf(radiance[i]) && !std::isnan(radiance[i]),
			std::to_string(radiance[i]));
	}*/

	return radiance;
}

template<CPhoton Photon>
inline const TPhotonMap<Photon>* TRadianceEvaluationWork<Photon>::getPhotonMap() const
{
	return m_photonMap;
}

template<CPhoton Photon>
inline const std::size_t TRadianceEvaluationWork<Photon>::numPhotonPaths() const
{
	return m_numPhotonPaths;
}

}// end namespace ph
