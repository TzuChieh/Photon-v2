#pragma once

#include "Common/primitive_type.h"
#include "Common/assertion.h"
#include "Core/Renderer/PM/TViewpointHandler.h"
#include "Core/Renderer/PM/TPhotonMap.h"
#include "Core/Renderer/PM/FullPhoton.h"
#include "Core/Filmic/HdrRgbFilm.h"
#include "Core/Intersectable/Primitive.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Core/SurfaceBehavior/SurfaceOptics.h"
#include "Core/SurfaceHit.h"
#include "Core/Emitter/Emitter.h"
#include "Core/Renderer/PM/PMRenderer.h"
#include "Core/LTABuildingBlock/TSurfaceEventDispatcher.h"
#include "Core/LTABuildingBlock/lta.h"

#include <vector>

namespace ph
{

class Scene;
class Camera;
class SampleGenerator;
class PMStatistics;
class PMRenderer;

class VPMRadianceEvaluator : public TViewpointHandler<VPMRadianceEvaluator>
{
public:
	VPMRadianceEvaluator(
		const TPhotonMap<FullPhoton>* photonMap,
		std::size_t                   numPhotonPaths,
		HdrRgbFilm*                   film);

	void impl_onPathStart(
		const Vector2R&         filmNdc,
		const SpectralStrength& pathThroughput);

	void impl_onPathSurfaceHit(
		std::size_t             nthSurfaceHit,
		const SurfaceHit&       surfaceHit,
		const SpectralStrength& pathThroughput);

	void impl_onPathEnd(
		std::size_t             numPathHits);

	void setPMStatistics(PMStatistics* statistics);
	void setPMRenderer(PMRenderer* renderer);
	void setKernelRadius(real radius);

private:
	const TPhotonMap<FullPhoton>* m_photonMap;
	std::size_t                   m_numPhotonPaths;
	HdrRgbFilm*                   m_film;

	real                          m_kernelRadius;
	PMStatistics*                 m_statistics;
	PMRenderer*                   m_renderer;

	Vector2R                      m_filmNdc;
	SpectralStrength              m_radiance;
};

// In-header Implementations:

inline VPMRadianceEvaluator::VPMRadianceEvaluator(
	const TPhotonMap<FullPhoton>* photonMap,
	std::size_t                   numPhotonPaths,
	HdrRgbFilm*                   film) :

	m_photonMap(photonMap),
	m_numPhotonPaths(numPhotonPaths),
	m_film(film)
{
	PH_ASSERT(photonMap);
	PH_ASSERT(film);
	PH_ASSERT_GT(numPhotonPaths, 0);

	setPMStatistics(nullptr);
	setPMRenderer(nullptr);
	setKernelRadius(0.1_r);
}

inline void VPMRadianceEvaluator::impl_onPathStart(
	const Vector2R&         filmNdc,
	const SpectralStrength& pathThroughput)
{

}

inline void VPMRadianceEvaluator::impl_onPathSurfaceHit(
	const std::size_t       nthSurfaceHit,
	const SurfaceHit&       surfaceHit,
	const SpectralStrength& pathThroughput)
{

}

inline void VPMRadianceEvaluator::impl_onPathEnd(
	const std::size_t       numPathHits)
{

}

inline void VPMRadianceEvaluator::setPMStatistics(PMStatistics* const statistics)
{
	m_statistics = statistics;
}

inline void VPMRadianceEvaluator::setPMRenderer(PMRenderer* const renderer)
{
	m_renderer = renderer;
}

inline void VPMRadianceEvaluator::setKernelRadius(const real radius)
{
	PH_ASSERT_GT(radius, 0.0_r);

	m_kernelRadius = radius;
}

}// end namespace ph