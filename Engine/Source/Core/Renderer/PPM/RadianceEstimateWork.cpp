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
	const real radius = 1.0_r;

	std::vector<Photon> photons;
	for(std::size_t i = 0; i < m_numViewpoints; ++i)
	{
		Viewpoint* viewpoint = &(m_viewpoints[i]);

		photons.clear();
		m_photonMap->findWithinRange(viewpoint->hit.getPosition(), radius, photons);

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
			throughput.mulLocal(photon.throughput);
			throughput.mulLocal(bsdf);
			throughput.mulLocal(Ns.absDot(L));
			throughput.mulLocal(Ns.absDot(V) * Ng.absDot(L) / Ng.absDot(V) / Ns.absDot(L));
			throughput.mulLocal(viewpoint->throughput);
		}
	}
}

}// end namespace ph