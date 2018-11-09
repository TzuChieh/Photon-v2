#pragma once

#include "Core/Renderer/PM/TRadianceEvaluationWork.h"
#include "Math/constant.h"
#include "Core/Intersectable/Primitive.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Core/SurfaceBehavior/SurfaceOptics.h"

#include <vector>

namespace ph
{

template<typename Photon>
inline TRadianceEvaluationWork<Photon>::TRadianceEvaluationWork(
	const TPhotonMap<Photon>* photonMap,
	Viewpoint* viewpoints,
	std::size_t numViewpoints,
	HdrRgbFilm* film,
	std::size_t numEmittedPhotons) :
	m_photonMap(photonMap),
	m_viewpoints(viewpoints),
	m_numViewpoints(numViewpoints),
	m_film(film),
	m_numEmittedPhotons(numEmittedPhotons)
{}

template<typename Photon>
inline void TRadianceEvaluationWork<Photon>::doWork()
{
	for(std::size_t i = 0; i < m_numViewpoints; ++i)
	{
		const Viewpoint& viewpoint = m_viewpoints[i];
		
		const SpectralStrength& radiance = evaluateRadiance(
			viewpoint.hit,
			viewpoint.L,
			viewpoint.radius);

		real filmXPx = viewpoint.filmNdcPos.x * static_cast<real>(m_film->getActualResPx().x);
		real filmYPx = viewpoint.filmNdcPos.y * static_cast<real>(m_film->getActualResPx().y);
		m_film->addSample(filmXPx, filmYPx, radiance);
	}
}

template<typename Photon>
inline SpectralStrength TRadianceEvaluationWork<Photon>::evaluateRadiance(
	const SurfaceHit& location,
	const Vector3R&   excitant,
	const real        kernelRadius)
{
	static_assert(
		Photon::has<EPhotonData::INCIDENT_DIR>() &&
		Photon::has<EPhotonData::RADIANCE>()     &&
		Photon::has<EPhotonData::THROUGHPUT>(),
		"provided Photon do not have sufficient data");

	const real reciKernelArea        = 1.0_r / (kernelRadius * kernelRadius * PH_PI_REAL);
	const real reciNumEmittedPhotons = 1.0_r / static_cast<real>(m_numEmittedPhotons);

	m_photonCache.clear();
	m_photonMap->findWithinRange(location.getPosition(), kernelRadius, m_photonCache);

	BsdfEvaluation   bsdfEval;
	SpectralStrength radiance(0);
	for(const auto& photon : m_photonCache)
	{
		const Vector3R V  = excitant;
		const Vector3R L  = photon.get<EPhotonData::INCIDENT_DIR>();
		const Vector3R Ng = location.getGeometryNormal();
		const Vector3R Ns = location.getShadingNormal();

		const PrimitiveMetadata* const metadata = location.getDetail().getPrimitive()->getMetadata();
		const SurfaceOptics*     const optics = metadata->getSurface().getOptics();

		bsdfEval.inputs.set(location, L, V, ALL_ELEMENTALS, ETransport::RADIANCE);
		optics->calcBsdf(bsdfEval);

		if(!bsdfEval.outputs.isGood())
		{
			continue;
		}

		SpectralStrength throughput(1.0_r);
		throughput.mulLocal(bsdfEval.outputs.bsdf);
		//throughput.mulLocal(Ns.absDot(V) * Ng.absDot(L) / Ng.absDot(V) / Ns.absDot(L));
		throughput.mulLocal(photon.get<EPhotonData::THROUGHPUT>());

		radiance.addLocal(throughput * photon.get<EPhotonData::RADIANCE>() * reciKernelArea * reciNumEmittedPhotons);
	}

	/*for(std::size_t i = 0; i < radiance.NUM_VALUES; ++i)
	{
		PH_ASSERT_MSG(!std::isinf(radiance[i]) && !std::isnan(radiance[i]),
			std::to_string(radiance[i]));
	}*/

	return radiance;
}

}// end namespace ph