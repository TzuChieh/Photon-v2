#include "Core/Renderer/PPM/RadianceEstimateWork.h"
#include "Math/constant.h"
#include "Core/Intersectable/Primitive.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Core/SurfaceBehavior/SurfaceOptics.h"

#include <vector>

namespace ph
{

RadianceEstimateWork::RadianceEstimateWork(
	const PhotonMap* photonMap,
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

void RadianceEstimateWork::doWork()
{
	std::vector<Photon> photons;
	for(std::size_t i = 0; i < m_numViewpoints; ++i)
	{
		Viewpoint* viewpoint = &(m_viewpoints[i]);

		photons.clear();
		m_photonMap->findWithinRange(viewpoint->hit.getPosition(), viewpoint->radius, photons);
		// DEBUG
		//m_photonMap->findWithinRange(viewpoint->hit.getPosition(), 0.004, photons);

		const real r = viewpoint->radius;
		SpectralStrength radiance(0);
		for(const auto& photon : photons)
		{
			const Vector3R V = photon.V;
			const Vector3R L = viewpoint->L;
			const Vector3R Ng = viewpoint->hit.getGeometryNormal();
			const Vector3R Ns = viewpoint->hit.getShadingNormal();
			
			const PrimitiveMetadata* metadata = viewpoint->hit.getDetail().getPrimitive()->getMetadata();
			const SurfaceOptics* optics = metadata->getSurface().getOptics();

			BsdfEvaluation bsdfEval;
			//bsdfEval.inputs.set(viewpoint->hit, L, V, ALL_ELEMENTALS, ETransport::IMPORTANCE);
			bsdfEval.inputs.set(viewpoint->hit, V, L, ALL_ELEMENTALS, ETransport::RADIANCE);
			optics->calcBsdf(bsdfEval);

			SpectralStrength bsdf(0);
			if(bsdfEval.outputs.isGood())
			{
				bsdf = bsdfEval.outputs.bsdf;
			}

			SpectralStrength throughput(1.0_r);
			throughput.mulLocal(viewpoint->throughput);
			//throughput.mulLocal(Ns.absDot(L));
			//throughput.mulLocal(Ns.absDot(V));
			throughput.mulLocal(bsdf);
			//throughput.mulLocal(Ns.absDot(V) * Ng.absDot(L) / Ng.absDot(V) / Ns.absDot(L));
			throughput.mulLocal(photon.throughput);

			PH_ASSERT_MSG(throughput.isNonNegative(), "throughput = " + throughput.toString());
			PH_ASSERT_MSG(photon.radiance.isNonNegative(), "photon.radiance = " + photon.radiance.toString());
			
			//radiance.addLocal(throughput * photon.radiance / (r * r * PH_PI_REAL) / static_cast<real>(m_photonMap->numItems()));
			radiance.addLocal(throughput * photon.radiance / (r * r * PH_PI_REAL) / static_cast<real>(m_numEmittedPhotons));
		}

		// DEBUG
		//radiance.setValues(static_cast<real>(photons.size()) / 10);
		//radiance.divLocal(2 * PH_PI_REAL);

		for(std::size_t i = 0; i < radiance.NUM_VALUES; ++i)
		{
			PH_ASSERT_MSG(!std::isinf(radiance[i]) && !std::isnan(radiance[i]), std::to_string(radiance[i]));
		}

		real filmXPx = viewpoint->filmNdcPos.x * static_cast<real>(m_film->getActualResPx().x);
		real filmYPx = viewpoint->filmNdcPos.y * static_cast<real>(m_film->getActualResPx().y);
		m_film->addSample(filmXPx, filmYPx, radiance);
	}
}

}// end namespace ph