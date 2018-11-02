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
	HdrRgbFilm* film) :
	m_photonMap(photonMap),
	m_viewpoints(viewpoints),
	m_numViewpoints(numViewpoints),
	m_film(film)
{}

void RadianceEstimateWork::doWork()
{
	std::vector<Photon> photons;
	for(std::size_t i = 0; i < m_numViewpoints; ++i)
	{
		Viewpoint* viewpoint = &(m_viewpoints[i]);

		photons.clear();
		m_photonMap->findWithinRange(viewpoint->hit.getPosition(), viewpoint->radius, photons);

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
			bsdfEval.inputs.set(viewpoint->hit, L, V, ALL_ELEMENTALS, ETransport::IMPORTANCE);
			optics->calcBsdf(bsdfEval);

			SpectralStrength bsdf(0);
			if(bsdfEval.outputs.isGood())
			{
				bsdf = bsdfEval.outputs.bsdf;
			}

			SpectralStrength throughput(1.0_r);
			throughput.mulLocal(viewpoint->throughput);
			throughput.mulLocal(Ns.absDot(L));
			throughput.mulLocal(bsdf);
			throughput.mulLocal(Ns.absDot(V) * Ng.absDot(L) / Ng.absDot(V) / Ns.absDot(L));
			throughput.mulLocal(photon.throughput);
			
			radiance.addLocal(throughput * photon.radiance / (r * r * PH_PI_REAL) / static_cast<real>(m_photonMap->numItems()));
		}
		real filmXPx = viewpoint->filmNdcPos.x * static_cast<real>(m_film->getActualResPx().x);
		real filmYPx = viewpoint->filmNdcPos.y * static_cast<real>(m_film->getActualResPx().y);
		m_film->addSample(filmXPx, filmYPx, radiance);
	}
}

}// end namespace ph