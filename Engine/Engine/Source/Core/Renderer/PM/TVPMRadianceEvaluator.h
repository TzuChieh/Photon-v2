#pragma once

#include "Core/Renderer/PM/TViewPathHandler.h"
#include "Core/Renderer/PM/TPhotonMap.h"
#include "Core/Renderer/PM/TPhoton.h"
#include "Core/Renderer/PM/PMCommonParams.h"
#include "Core/Filmic/TSamplingFilm.h"
#include "Core/SurfaceBehavior/SurfaceOptics.h"
#include "Core/SurfaceHit.h"
#include "Core/SurfaceBehavior/BsdfQueryContext.h"
#include "Core/SurfaceBehavior/BsdfEvalQuery.h"
#include "Core/Renderer/PM/photon_map_light_transport.h"
#include "Math/Color/Spectrum.h"

#include <Common/assertion.h>
#include <Common/primitive_type.h>

#include <cstddef>
#include <vector>

namespace ph { class Scene; }
namespace ph { class PMAtomicStatistics; }

namespace ph
{

template<CPhoton Photon, typename PhotonMap = TPhotonMap<Photon>>
class TVPMRadianceEvaluator : public TViewPathHandler<TVPMRadianceEvaluator<Photon, PhotonMap>>
{
public:
	TVPMRadianceEvaluator(
		const PhotonMap*               photonMap,
		const Scene*                   scene,
		TSamplingFilm<math::Spectrum>* film);

	bool impl_onReceiverSampleStart(
		const math::Vector2D&          rasterCoord,
		const math::Vector2S&          sampleIndex,
		const math::Spectrum&          pathThroughput);

	auto impl_onPathHitSurface(
		std::size_t                    pathLength,
		const SurfaceHit&              surfaceHit,
		const math::Spectrum&          pathThroughput) -> ViewPathTracingPolicy;

	void impl_onReceiverSampleEnd();

	void impl_onSampleBatchFinished();

	/*! @brief Binds a statistics tracker.
	This evaluator will increment the number of steps for each processed sample batch.
	*/
	void setStatistics(PMAtomicStatistics* statistics);

	void setKernelRadius(real radius);

	/*! @brief Set the view path length to start random path sampling.
	*/
	void setStochasticSampleBeginLength(std::size_t stochasticSampleBeginLength);

	/*! @brief Set the full light transport path lengths for radiance evaluation.
	Whether all the requested path lengths are evaluated using photon map also depends on the input
	photon map's settings. Path lengths must be >= 1.
	@param minFullPathLength The minimum full light transport path length to consider.
	@param maxFullPathLength The maximum full light transport path length to consider (inclusive).
	*/
	void setFullPathLengthRange(
		std::size_t minFullPathLength, 
		std::size_t maxFullPathLength = PMCommonParams::DEFAULT_MAX_PATH_LENGTH);

	void setGlossyMergeBeginLength(uint32 glossyMergeBeginLength);

private:
	math::Spectrum estimateRadianceWithPhotonMap(
		const SurfaceHit& X,
		const BsdfQueryContext& bsdfContext,
		const math::Spectrum& viewPathThroughput);

	const PhotonMap*               m_photonMap;
	TSamplingFilm<math::Spectrum>* m_film;
	const Scene*                   m_scene;

	real                           m_kernelRadius;
	real                           m_kernelDensityNormalizer;
	PMAtomicStatistics*            m_statistics;
	std::size_t                    m_stochasticSampleBeginLength;
	std::size_t                    m_minFullPathLength;
	std::size_t                    m_maxFullPathLength;
	uint32                         m_glossyMergeBeginLength;

	math::Vector2D                 m_rasterCoord;
	math::Spectrum                 m_sampledRadiance;
	std::vector<Photon>            m_photonCache;
};

// In-header Implementations:

template<CPhoton Photon, typename PhotonMap>
inline TVPMRadianceEvaluator<Photon, PhotonMap>
::TVPMRadianceEvaluator(
	const PhotonMap* const               photonMap,
	const Scene* const                   scene,
	TSamplingFilm<math::Spectrum>* const film)

	: m_photonMap                  (photonMap)
	, m_film                       (film)
	, m_scene                      (scene)

	, m_kernelRadius               ()
	, m_kernelDensityNormalizer    ()
	, m_statistics                 ()
	, m_stochasticSampleBeginLength()
	, m_minFullPathLength          ()
	, m_maxFullPathLength          ()
	, m_glossyMergeBeginLength     ()

	, m_rasterCoord                ()
	, m_sampledRadiance            ()
	, m_photonCache                ()
{
	PH_ASSERT(photonMap);
	PH_ASSERT_GT(photonMap->numPaths, 0);
	PH_ASSERT(scene);
	PH_ASSERT(film);

	setStatistics(nullptr);
	setKernelRadius(0.1_r);
	setStochasticSampleBeginLength(1);
	setFullPathLengthRange(1);
	setGlossyMergeBeginLength(1);
}

template<CPhoton Photon, typename PhotonMap>
inline bool TVPMRadianceEvaluator<Photon, PhotonMap>
::impl_onReceiverSampleStart(
	const math::Vector2D& rasterCoord,
	const math::Vector2S& sampleIndex,
	const math::Spectrum& pathThroughput)
{
	m_rasterCoord = rasterCoord;
	m_sampledRadiance.setColorValues(0);

	return true;
}

template<CPhoton Photon, typename PhotonMap>
inline auto TVPMRadianceEvaluator<Photon, PhotonMap>
::impl_onPathHitSurface(
	const std::size_t     pathLength,
	const SurfaceHit&     surfaceHit,
	const math::Spectrum& pathThroughput)
-> ViewPathTracingPolicy
{
	const SurfaceOptics* optics = surfaceHit.getSurfaceOptics();
	if(!optics)
	{
		return ViewPathTracingPolicy().kill();
	}

	const auto unaccountedEnergy = estimate_certainly_lost_energy(
		pathLength,
		surfaceHit,
		pathThroughput,
		m_photonMap->getInfo(),
		m_scene,
		m_minFullPathLength,
		m_maxFullPathLength);
	m_sampledRadiance += unaccountedEnergy;

	const auto smoothEnoughPhenomena = {
		DIFFUSE_SURFACE_PHENOMENA,
		ESurfacePhenomenon::NearDiffuseReflection,
		ESurfacePhenomenon::NearDiffuseTransmission};

	const auto phenomena = optics->getAllPhenomena();
	const bool isSufficientlyDiffuse = pathLength >= m_glossyMergeBeginLength
		? phenomena.hasAny(smoothEnoughPhenomena)
		: phenomena.hasExactly(DIFFUSE_SURFACE_PHENOMENA);

	if(m_photonMap->canContribute(pathLength, m_minFullPathLength, m_maxFullPathLength) &&
	   phenomena.hasNone(DELTA_SURFACE_PHENOMENA) &&
	   isSufficientlyDiffuse)
	{
		const BsdfQueryContext bsdfContext(
			ALL_SURFACE_ELEMENTALS, lta::ETransport::Importance, lta::ESidednessPolicy::Strict);

		// For path length = N, we can construct light transport path lengths with photon map,
		// all at once, for the range [N_min, N_max] = 
		// [`N + m_photonMap->minPathLength`, `N + m_photonMap->maxPathLength`].
		m_sampledRadiance += estimateRadianceWithPhotonMap(
			surfaceHit, bsdfContext, pathThroughput);

		const auto unaccountedEnergy = estimate_lost_energy_for_merging(
			pathLength,
			surfaceHit,
			pathThroughput,
			m_photonMap->getInfo(),
			m_scene,
			m_minFullPathLength,
			m_maxFullPathLength);
		m_sampledRadiance += unaccountedEnergy;

		return ViewPathTracingPolicy().kill();
	}
	else
	{
		const auto unaccountedEnergy = estimate_lost_energy_for_extending(
			pathLength,
			surfaceHit,
			pathThroughput,
			m_photonMap->getInfo(),
			m_scene,
			m_minFullPathLength,
			m_maxFullPathLength);
		m_sampledRadiance += unaccountedEnergy;

		if(pathLength < m_stochasticSampleBeginLength)
		{
			return ViewPathTracingPolicy().
				traceBranchedPathFor(SurfacePhenomena(ALL_SURFACE_PHENOMENA)).
				useRussianRoulette(false);
		}
		else
		{
			return ViewPathTracingPolicy().
				traceSinglePathFor(ALL_SURFACE_ELEMENTALS).
				useRussianRoulette(true);
		}
	}
}

template<CPhoton Photon, typename PhotonMap>
inline void TVPMRadianceEvaluator<Photon, PhotonMap>
::impl_onReceiverSampleEnd()
{
	m_film->addSample(m_rasterCoord.x(), m_rasterCoord.y(), m_sampledRadiance);
}

template<CPhoton Photon, typename PhotonMap>
inline void TVPMRadianceEvaluator<Photon, PhotonMap>
::impl_onSampleBatchFinished()
{
	if(m_statistics)
	{
		m_statistics->incrementNumProcessedSteps();
	}
}

template<CPhoton Photon, typename PhotonMap>
inline math::Spectrum TVPMRadianceEvaluator<Photon, PhotonMap>
::estimateRadianceWithPhotonMap(
	const SurfaceHit& X,
	const BsdfQueryContext& bsdfContext,
	const math::Spectrum& viewPathThroughput)
{
	m_photonCache.clear();
	m_photonMap->find(X.getPos(), m_kernelRadius, m_photonCache);

	const lta::SurfaceTracer surfaceTracer{m_scene};

	const math::Vector3R L  = X.getIncidentRay().getDir().mul(-1);
	const math::Vector3R Ns = X.getShadingNormal();
	const math::Vector3R Ng = X.getGeometryNormal();

	BsdfEvalQuery  bsdfEval(bsdfContext);
	math::Spectrum radiance(0);
	for(const auto& photon : m_photonCache)
	{
		const math::Vector3R V = photon.template get<EPhotonData::FromDir>();
		if(!accept_photon_by_surface_topology(photon, Ng, Ns, L, V, bsdfContext.sidedness))
		{
			continue;
		}

		bsdfEval.inputs.set(X, L, V);
		if(!surfaceTracer.doBsdfEvaluation(bsdfEval))
		{
			continue;
		}

		math::Spectrum throughput(viewPathThroughput);
		throughput.mulLocal(bsdfEval.outputs.getBsdf());
		throughput.mulLocal(lta::tamed_importance_BSDF_Ns_corrector(Ns, Ng, V));

		radiance.addLocal(throughput * photon.template get<EPhotonData::ThroughputRadiance>());
	}
	radiance.mulLocal(m_kernelDensityNormalizer);

	return radiance;
}

template<CPhoton Photon, typename PhotonMap>
inline void TVPMRadianceEvaluator<Photon, PhotonMap>
::setStatistics(PMAtomicStatistics* const statistics)
{
	m_statistics = statistics;
}

template<CPhoton Photon, typename PhotonMap>
inline void TVPMRadianceEvaluator<Photon, PhotonMap>
::setKernelRadius(const real radius)
{
	PH_ASSERT_GT(radius, 0.0_r);

	m_kernelRadius = radius;

	const real kernelArea = radius * radius * math::constant::pi<real>;
	m_kernelDensityNormalizer = 1.0_r / (kernelArea * static_cast<real>(m_photonMap->numPaths));
}

template<CPhoton Photon, typename PhotonMap>
inline void TVPMRadianceEvaluator<Photon, PhotonMap>
::setStochasticSampleBeginLength(
	const std::size_t stochasticSampleBeginLength)
{
	PH_ASSERT_GE(stochasticSampleBeginLength, 1);

	m_stochasticSampleBeginLength = stochasticSampleBeginLength;
}

template<CPhoton Photon, typename PhotonMap>
inline void TVPMRadianceEvaluator<Photon, PhotonMap>
::setFullPathLengthRange(
	const std::size_t minFullPathLength, const std::size_t maxFullPathLength)
{
	PH_ASSERT_GE(minFullPathLength, 1);
	PH_ASSERT_LE(minFullPathLength, maxFullPathLength);

	m_minFullPathLength = minFullPathLength;
	m_maxFullPathLength = maxFullPathLength;
}

template<CPhoton Photon, typename PhotonMap>
inline void TVPMRadianceEvaluator<Photon, PhotonMap>
::setGlossyMergeBeginLength(
	const uint32 glossyMergeBeginLength)
{
	PH_ASSERT_GE(glossyMergeBeginLength, 1);

	m_glossyMergeBeginLength = glossyMergeBeginLength;
}

}// end namespace ph
