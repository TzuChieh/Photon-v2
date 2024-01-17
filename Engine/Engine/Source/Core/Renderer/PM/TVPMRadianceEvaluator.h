#pragma once

#include "Core/Renderer/PM/TViewPathHandler.h"
#include "Core/Renderer/PM/TPhotonMap.h"
#include "Core/Renderer/PM/TPhoton.h"
#include "Core/Filmic/TSamplingFilm.h"
#include "Core/Intersectable/Primitive.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Core/SurfaceBehavior/SurfaceOptics.h"
#include "Core/SurfaceHit.h"
#include "Core/Emitter/Emitter.h"
#include "Core/LTA/SurfaceTracer.h"
#include "Core/LTA/lta.h"
#include "Core/LTA/TDirectLightEstimator.h"
#include "Core/LTA/TIndirectLightEstimator.h"
#include "Core/SurfaceBehavior/BsdfQueryContext.h"
#include "Core/SurfaceBehavior/BsdfEvalQuery.h"
#include "Math/Color/Spectrum.h"

#include <Common/assertion.h>
#include <Common/primitive_type.h>

#include <cstddef>
#include <vector>

namespace ph
{

class Scene;
class PMAtomicStatistics;

template<CPhoton Photon>
class TVPMRadianceEvaluator : public TViewPathHandler<TVPMRadianceEvaluator<Photon>>
{
public:
	TVPMRadianceEvaluator(
		const TPhotonMap<Photon>*      photonMap,
		std::size_t                    numPhotonPaths,
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
	void setStochasticPathSampleBeginLength(std::size_t stochasticSampleBeginLength);

	/*! @brief Set the view path lengths for radiance evaluation.
	This is not the range of full path lengths. The range of full path lengths that are being
	evaluated also depends on the photon map used.
	*/
	void setPathLengthRange(std::size_t evalBeginLength, std::size_t evalEndLength = 16384);

private:
	math::Spectrum estimateRadianceWithPhotonMap(
		const SurfaceHit& X,
		const BsdfQueryContext& bsdfContext,
		const math::Spectrum& viewPathThroughput);

	const TPhotonMap<Photon>*      m_photonMap;
	std::size_t                    m_numPhotonPaths;
	TSamplingFilm<math::Spectrum>* m_film;
	const Scene*                   m_scene;

	real                           m_kernelRadius;
	real                           m_kernelDensityNormalizer;
	PMAtomicStatistics*            m_statistics;
	std::size_t                    m_stochasticSampleBeginLength;
	std::size_t                    m_evalBeginLength;
	std::size_t                    m_evalEndLength;

	math::Vector2D                 m_rasterCoord;
	math::Spectrum                 m_sampledRadiance;
	std::vector<Photon>            m_photonCache;
};

// In-header Implementations:

template<CPhoton Photon>
inline TVPMRadianceEvaluator<Photon>::TVPMRadianceEvaluator(
	const TPhotonMap<Photon>* const      photonMap,
	const std::size_t                    numPhotonPaths,
	const Scene* const                   scene,
	TSamplingFilm<math::Spectrum>* const film)

	: m_photonMap(photonMap)
	, m_numPhotonPaths(numPhotonPaths)
	, m_film(film)
	, m_scene(scene)

	, m_kernelRadius()
	, m_kernelDensityNormalizer()
	, m_statistics()
	, m_stochasticSampleBeginLength()
	, m_evalBeginLength()
	, m_evalEndLength()

	, m_rasterCoord()
	, m_sampledRadiance()
	, m_photonCache()
{
	PH_ASSERT(photonMap);
	PH_ASSERT_GT(numPhotonPaths, 0);
	PH_ASSERT(scene);
	PH_ASSERT(film);

	setStatistics(nullptr);
	setKernelRadius(0.1_r);
	setStochasticPathSampleBeginLength(1);
	setPathLengthRange(1);
}

template<CPhoton Photon>
inline bool TVPMRadianceEvaluator<Photon>::impl_onReceiverSampleStart(
	const math::Vector2D& rasterCoord,
	const math::Vector2S& sampleIndex,
	const math::Spectrum& pathThroughput)
{
	m_rasterCoord = rasterCoord;
	m_sampledRadiance.setColorValues(0);

	return true;
}

template<CPhoton Photon>
inline auto TVPMRadianceEvaluator<Photon>::impl_onPathHitSurface(
	const std::size_t     pathLength,
	const SurfaceHit&     surfaceHit,
	const math::Spectrum& pathThroughput) -> ViewPathTracingPolicy
{
	using DirectLight = lta::TDirectLightEstimator<lta::ESidednessPolicy::Strict>;
	using IndirectLight = lta::TIndirectLightEstimator<lta::ESidednessPolicy::Strict>;

	PH_ASSERT_GE(pathLength, 1);

	PH_ASSERT(surfaceHit.getDetail().getPrimitive());
	const PrimitiveMetadata* meta = surfaceHit.getDetail().getPrimitive()->getMetadata();
	const SurfaceOptics* optics = meta ? meta->getSurface().getOptics() : nullptr;
	if(pathLength + 1 == m_evalEndLength || !optics)
	{
		return ViewPathTracingPolicy().kill();
	}

	// Not evaluating radiance for current path length yet
	if(pathLength < m_evalBeginLength)
	{
		if(pathLength < m_stochasticSampleBeginLength)
		{
			return ViewPathTracingPolicy().
				traceBranchedPathFor(SurfacePhenomena({ALL_SURFACE_PHENOMENA})).
				useRussianRoulette(false);
		}
		else
		{
			return ViewPathTracingPolicy().
				traceSinglePathFor(ALL_SURFACE_ELEMENTALS).
				useRussianRoulette(true);
		}
	}

	// Evaluate radiance for current path length
	PH_ASSERT_IN_RANGE(pathLength, m_evalBeginLength, m_evalEndLength);

	// Cannot have path length = 1 lighting using only photon map--when we use a photon map, it is
	// at least path length = 2 (can be even longer depending on the settings)

	// Never contain 0-bounce photons
	PH_ASSERT_GE(m_photonMap->minPhotonPathLength, 1);

	// Path length = 1 (0-bounce) lighting via path tracing (directly sample radiance)
	if(pathLength == 1 && meta->getSurface().getEmitter())
	{
		math::Spectrum viewRadiance;
		meta->getSurface().getEmitter()->evalEmittedRadiance(surfaceHit, &viewRadiance);
		m_sampledRadiance.addLocal(pathThroughput * viewRadiance);
	}

	// +1 as when we merge view path with photon path, full path length is at least increased by 1
	const auto minFullLengthWithPhotonMap = m_photonMap->minPhotonPathLength + 1;

	// Path length > 1 lighting via path tracing if we cannot construct the path length
	// from photon map
	if(pathLength + 1 < minFullLengthWithPhotonMap)
	{
		math::Spectrum viewRadiance;
		SampleFlow randomFlow;
		if(DirectLight{m_scene}.bsdfSampleOutgoingWithNee(
			surfaceHit, 
			randomFlow,
			&viewRadiance))
		{
			m_sampledRadiance.addLocal(pathThroughput * viewRadiance);
		}
	}

	// FIXME: properly handle delta optics (mixed case)

	// For path length = N, we can construct full light transport path lengths with photon map, all at
	// once, for the range [`N + m_photonMap->minPhotonPathLength`, infinity (if RR is used))
	if(optics->getAllPhenomena().hasNo(DELTA_SURFACE_PHENOMENA) &&
	   optics->getAllPhenomena().hasAny({ESurfacePhenomenon::DiffuseReflection}))
	{
		const BsdfQueryContext bsdfContext(
			ALL_SURFACE_ELEMENTALS, ETransport::Importance, lta::ESidednessPolicy::Strict);

		m_sampledRadiance.addLocal(estimateRadianceWithPhotonMap(
			surfaceHit, bsdfContext, pathThroughput));

		return ViewPathTracingPolicy().kill();
	}
	else
	{
		// If we extend the path length from N (current) to N + 1 (by returning a non-killing policy),
		// this means we are not using photon map to approximate lighting for path length = N' =
		// `N + m_photonMap->minPhotonPathLength`. We will lose energy for path length = N' if we do
		// nothing. Here we use path tracing to find the energy that would otherwise be lost.
		math::Spectrum viewRadiance;
		SampleFlow randomFlow;
		if(IndirectLight{m_scene}.bsdfSamplePathWithNee(
			surfaceHit, 
			randomFlow,
			m_photonMap->minPhotonPathLength,// we are already on view path of length N
			lta::RussianRoulette{},
			&viewRadiance))
		{
			m_sampledRadiance.addLocal(pathThroughput * viewRadiance);
		}

		if(pathLength < m_stochasticSampleBeginLength)
		{
			return ViewPathTracingPolicy().
				traceBranchedPathFor(SurfacePhenomena({ALL_SURFACE_PHENOMENA})).
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

template<CPhoton Photon>
inline void TVPMRadianceEvaluator<Photon>::impl_onReceiverSampleEnd()
{
	m_film->addSample(m_rasterCoord.x(), m_rasterCoord.y(), m_sampledRadiance);
}

template<CPhoton Photon>
inline void TVPMRadianceEvaluator<Photon>::impl_onSampleBatchFinished()
{
	if(m_statistics)
	{
		m_statistics->incrementNumProcessedSteps();
	}
}

template<CPhoton Photon>
inline math::Spectrum TVPMRadianceEvaluator<Photon>::estimateRadianceWithPhotonMap(
	const SurfaceHit& X,
	const BsdfQueryContext& bsdfContext,
	const math::Spectrum& viewPathThroughput)
{
	m_photonCache.clear();
	m_photonMap->map.findWithinRange(X.getPosition(), m_kernelRadius, m_photonCache);

	const lta::SurfaceTracer surfaceTracer{m_scene};

	const math::Vector3R L  = X.getIncidentRay().getDirection().mul(-1);
	const math::Vector3R Ns = X.getShadingNormal();
	const math::Vector3R Ng = X.getGeometryNormal();

	BsdfEvalQuery  bsdfEval(bsdfContext);
	math::Spectrum radiance(0);
	for(const auto& photon : m_photonCache)
	{
		const math::Vector3R V = photon.get<EPhotonData::FromDir>();
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
		throughput.mulLocal(bsdfEval.outputs.bsdf);
		throughput.mulLocal(lta::tamed_importance_BSDF_Ns_corrector(Ns, Ng, V));

		radiance.addLocal(throughput * photon.get<EPhotonData::ThroughputRadiance>());
	}
	radiance.mulLocal(m_kernelDensityNormalizer);

	return radiance;
}

template<CPhoton Photon>
inline void TVPMRadianceEvaluator<Photon>::setStatistics(PMAtomicStatistics* const statistics)
{
	m_statistics = statistics;
}

template<CPhoton Photon>
inline void TVPMRadianceEvaluator<Photon>::setKernelRadius(const real radius)
{
	PH_ASSERT_GT(radius, 0.0_r);

	m_kernelRadius = radius;

	const real kernelArea = radius * radius * math::constant::pi<real>;
	m_kernelDensityNormalizer = 1.0_r / (kernelArea * static_cast<real>(m_numPhotonPaths));
}

template<CPhoton Photon>
inline void TVPMRadianceEvaluator<Photon>::setStochasticPathSampleBeginLength(
	const std::size_t stochasticSampleBeginLength)
{
	PH_ASSERT_GE(stochasticSampleBeginLength, 1);

	m_stochasticSampleBeginLength = stochasticSampleBeginLength;
}

template<CPhoton Photon>
inline void TVPMRadianceEvaluator<Photon>::setPathLengthRange(
	const std::size_t evalBeginLength, const std::size_t evalEndLength)
{
	PH_ASSERT_LT(evalBeginLength, evalEndLength);

	m_evalBeginLength = evalBeginLength;
	m_evalEndLength = evalEndLength;
}

}// end namespace ph
