#pragma once

#include "Core/Renderer/RenderWork.h"
#include "Core/Renderer/PM/PhotonMap.h"
#include "Core/Renderer/PM/Viewpoint.h"
#include "Core/Filmic/HdrRgbFilm.h"
#include "Core/Intersectable/Primitive.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Core/SurfaceBehavior/SurfaceOptics.h"

namespace ph
{

class RadianceEstimateWork : public RenderWork
{
public:
	RadianceEstimateWork(
		const PhotonMap* photonMap,
		Viewpoint* viewpoints,
		std::size_t numViewpoints,
		HdrRgbFilm* film,
		std::size_t numEmittedPhotons);

	void doWork() override;

	SpectralStrength evaluateRadiance(
		const SurfaceHit& location,
		const Vector3R&   excitant,
		real              kernelRadius);

private:
	const PhotonMap* m_photonMap;
	Viewpoint* m_viewpoints;
	std::size_t m_numViewpoints;
	HdrRgbFilm* m_film;
	std::size_t m_numEmittedPhotons;
	std::vector<Photon> m_photonCache;
};

// In-header Implementations:

inline SpectralStrength RadianceEstimateWork::evaluateRadiance(
	const SurfaceHit& location,
	const Vector3R&   excitant,
	const real        kernelRadius)
{
	const real reciKernelArea        = 1.0_r / (kernelRadius * kernelRadius * PH_PI_REAL);
	const real reciNumEmittedPhotons = 1.0_r / static_cast<real>(m_numEmittedPhotons);

	m_photonCache.clear();
	m_photonMap->findWithinRange(location.getPosition(), kernelRadius, m_photonCache);

	BsdfEvaluation   bsdfEval;
	SpectralStrength radiance(0);
	for(const auto& photon : m_photonCache)
	{
		const Vector3R V  = excitant;
		const Vector3R L  = photon.V;
		const Vector3R Ng = location.getGeometryNormal();
		const Vector3R Ns = location.getShadingNormal();

		const PrimitiveMetadata* const metadata = location.getDetail().getPrimitive()->getMetadata();
		const SurfaceOptics*     const optics   = metadata->getSurface().getOptics();

		bsdfEval.inputs.set(location, L, V, ALL_ELEMENTALS, ETransport::RADIANCE);
		optics->calcBsdf(bsdfEval);

		if(!bsdfEval.outputs.isGood())
		{
			continue;
		}

		SpectralStrength throughput(1.0_r);
		throughput.mulLocal(bsdfEval.outputs.bsdf);
		//throughput.mulLocal(Ns.absDot(V) * Ng.absDot(L) / Ng.absDot(V) / Ns.absDot(L));
		throughput.mulLocal(photon.throughput);

		radiance.addLocal(throughput * photon.radiance * reciKernelArea * reciNumEmittedPhotons);
	}

	/*for(std::size_t i = 0; i < radiance.NUM_VALUES; ++i)
	{
		PH_ASSERT_MSG(!std::isinf(radiance[i]) && !std::isnan(radiance[i]), 
			std::to_string(radiance[i]));
	}*/

	return radiance;
}

}// end namespace ph