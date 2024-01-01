#pragma once

#include "Core/Renderer/PM/TViewPathHandler.h"
#include "Core/Renderer/PM/TPhotonMap.h"
#include "Core/Renderer/PM/FullPhoton.h"
#include "Core/Filmic/TSamplingFilm.h"
#include "Core/Intersectable/Primitive.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Core/SurfaceBehavior/SurfaceOptics.h"
#include "Core/SurfaceHit.h"
#include "Core/Emitter/Emitter.h"
#include "Core/LTA/SurfaceTracer.h"
#include "Core/LTA/lta.h"
#include "Core/SurfaceBehavior/BsdfQueryContext.h"
#include "Core/SurfaceBehavior/BsdfEvalQuery.h"
#include "Math/Color/Spectrum.h"

#include <Common/assertion.h>
#include <Common/primitive_type.h>

#include <vector>

namespace ph
{

class Scene;
class SampleGenerator;
class PMAtomicStatistics;

class VPMRadianceEvaluator : public TViewPathHandler<VPMRadianceEvaluator>
{
public:
	VPMRadianceEvaluator(
		const TPhotonMap<FullPhoton>*  photonMap,
		std::size_t                    numPhotonPaths,
		TSamplingFilm<math::Spectrum>* film,
		const Scene*                   scene);

	bool impl_onReceiverSampleStart(
		const math::Vector2D& rasterCoord,
		const math::Vector2S& sampleIndex,
		const math::Spectrum& pathThroughput);

	auto impl_onPathHitSurface(
		std::size_t           pathLength,
		const SurfaceHit&     surfaceHit,
		const math::Spectrum& pathThroughput) -> ViewPathTracingPolicy;

	void impl_onReceiverSampleEnd();

	void impl_onSampleBatchFinished();

	void setStatistics(PMAtomicStatistics* statistics);
	void setKernelRadius(real radius);

private:
	const TPhotonMap<FullPhoton>*  m_photonMap;
	std::size_t                    m_numPhotonPaths;
	TSamplingFilm<math::Spectrum>* m_film;
	const Scene*                   m_scene;

	real                           m_kernelRadius;
	PMAtomicStatistics*            m_statistics;

	math::Vector2D                 m_rasterCoord;
	math::Spectrum                 m_sampledRadiance;
	std::vector<FullPhoton>        m_photonCache;
};

// In-header Implementations:

inline VPMRadianceEvaluator::VPMRadianceEvaluator(
	const TPhotonMap<FullPhoton>* const  photonMap,
	const std::size_t                    numPhotonPaths,
	TSamplingFilm<math::Spectrum>* const film,
	const Scene* const                   scene)

	: m_photonMap(photonMap)
	, m_numPhotonPaths(numPhotonPaths)
	, m_film(film)
	, m_scene(scene)
{
	PH_ASSERT(photonMap);
	PH_ASSERT(film);
	PH_ASSERT_GT(numPhotonPaths, 0);

	setStatistics(nullptr);
	setKernelRadius(0.1_r);
}

inline bool VPMRadianceEvaluator::impl_onReceiverSampleStart(
	const math::Vector2D& rasterCoord,
	const math::Vector2S& sampleIndex,
	const math::Spectrum& pathThroughput)
{
	m_rasterCoord = rasterCoord;
	m_sampledRadiance.setColorValues(0);

	return true;
}

inline auto VPMRadianceEvaluator::impl_onPathHitSurface(
	const std::size_t     pathLength,
	const SurfaceHit&     surfaceHit,
	const math::Spectrum& pathThroughput) -> ViewPathTracingPolicy
{
	// TODO: remove hardcoded max path length
	constexpr std::size_t MAX_PATH_LENGTH = 5;

	PH_ASSERT_GE(pathLength, 1);

	const BsdfQueryContext bsdfContext(ALL_ELEMENTALS, ETransport::Importance, lta::ESidednessPolicy::Strict);
	const PrimitiveMetadata* const metadata      = surfaceHit.getDetail().getPrimitive()->getMetadata();
	const SurfaceOptics* const     surfaceOptics = metadata->getSurface().getOptics();

	if(metadata->getSurface().getEmitter())
	{
		math::Spectrum viewPathRadiance;
		metadata->getSurface().getEmitter()->evalEmittedRadiance(surfaceHit, &viewPathRadiance);
		m_sampledRadiance.addLocal(pathThroughput * viewPathRadiance);
	}

	// FIXME: properly handle delta optics (mixed case)

	if(pathLength < MAX_PATH_LENGTH && surfaceOptics->getAllPhenomena().hasAny({
		ESurfacePhenomenon::DeltaReflection,
		ESurfacePhenomenon::DeltaTransmission}))
	{
		return ViewPathTracingPolicy().
			traceBranchedPathFor(SurfacePhenomena({
				ESurfacePhenomenon::DeltaReflection,
				ESurfacePhenomenon::DeltaTransmission})).
			useRussianRoulette(false);
	}

	PH_ASSERT_LE(pathLength, MAX_PATH_LENGTH);

	m_photonCache.clear();
	m_photonMap->map.findWithinRange(surfaceHit.getPosition(), m_kernelRadius, m_photonCache);

	const lta::SurfaceTracer surfaceTracer{m_scene};

	const math::Vector3R L  = surfaceHit.getIncidentRay().getDirection().mul(-1);
	const math::Vector3R Ns = surfaceHit.getShadingNormal();
	const math::Vector3R Ng = surfaceHit.getGeometryNormal();

	BsdfEvalQuery  bsdfEval(bsdfContext);
	math::Spectrum radiance(0);
	for(const auto& photon : m_photonCache)
	{
		const math::Vector3R V = photon.get<EPhotonData::FromDir>();

		bsdfEval.inputs.set(surfaceHit, L, V);
		if(!surfaceTracer.doBsdfEvaluation(bsdfEval))
		{
			continue;
		}

		math::Spectrum throughput(pathThroughput);
		throughput.mulLocal(bsdfEval.outputs.bsdf);
		throughput.mulLocal(lta::importance_BSDF_Ns_corrector(Ns, Ng, L, V));

		radiance.addLocal(throughput * photon.get<EPhotonData::ThroughputRadiance>());
	}

	// OPT: cache
	const real kernelArea = m_kernelRadius * m_kernelRadius * math::constant::pi<real>;
	const real radianceMultiplier = 1.0_r / (kernelArea * static_cast<real>(m_numPhotonPaths));

	radiance.mulLocal(radianceMultiplier);

	m_sampledRadiance.addLocal(radiance);

	return ViewPathTracingPolicy().kill();
}

inline void VPMRadianceEvaluator::impl_onReceiverSampleEnd()
{
	m_film->addSample(m_rasterCoord.x(), m_rasterCoord.y(), m_sampledRadiance);
}

inline void VPMRadianceEvaluator::impl_onSampleBatchFinished()
{
	if(m_statistics)
	{
		m_statistics->incrementNumIterations();
	}
}

inline void VPMRadianceEvaluator::setStatistics(PMAtomicStatistics* const statistics)
{
	m_statistics = statistics;
}

inline void VPMRadianceEvaluator::setKernelRadius(const real radius)
{
	PH_ASSERT_GT(radius, 0.0_r);

	m_kernelRadius = radius;
}

}// end namespace ph
