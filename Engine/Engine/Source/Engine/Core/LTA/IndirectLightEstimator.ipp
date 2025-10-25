#pragma once

#include "Engine/Core/LTA/IndirectLightEstimator.h"
#include "Engine/Core/LTA/DirectLightEstimator.h"
#include "Engine/Core/LTA/SurfaceTracer.h"
#include "Engine/Core/SurfaceBehavior/BsdfQueryContext.h"
#include "Engine/Core/SurfaceBehavior/BsdfSampleQuery.h"
#include "Engine/Core/SurfaceHit.h"
#include "Engine/Core/Ray.h"
#include "Engine/Math/TVector3.h"

#include <Common/assertion.h>

namespace ph::lta
{

inline IndirectLightEstimator::IndirectLightEstimator(
	const Scene* const scene,
	const SurfacePhenomena& directPhenomenaMask,
	const SurfacePhenomena& indirectPhenomenaMask,
	const RussianRoulette& rr,
	const std::size_t rrBeginPathLength)

	: m_scene(scene)
	, m_directPhenomenaMask(directPhenomenaMask)
	, m_indirectPhenomenaMask(indirectPhenomenaMask)
	, m_rr(rr)
	, m_rrBeginPathLength(rrBeginPathLength)
{
	PH_ASSERT(scene);
}

inline bool IndirectLightEstimator::bsdfSampleSurfacePathWithNee(
	const SurfaceHit&       X,
	const BsdfQueryContext& baseContext,
	SampleFlow&             sampleFlow,
	const std::size_t       pathLength,
	math::Spectrum* const   out_Lo,
	const math::Spectrum&   initialPathWeight) const
{
	PH_ASSERT_GE(pathLength, 1);

	const SurfaceTracer surfaceTracer{m_scene};
	const DirectLightEstimator directLight{m_scene, baseContext.sidedness};

	SurfaceHit currentHit = X;
	BsdfQueryContext currentCtx = baseContext;
	math::Spectrum pathThroughput = initialPathWeight;
	real rrScale = 1.0_r;
	std::size_t currentPathLength = 0;
	while(currentPathLength < pathLength)// can extend by 1?
	{
		if(currentPathLength >= m_rrBeginPathLength)
		{
			real rrSurvivalProb;
			if(m_rr.surviveOnLuminance(pathThroughput * rrScale, sampleFlow, &rrSurvivalProb))
			{
				pathThroughput *= 1.0_r / rrSurvivalProb;
			}
			else
			{
				return false;
			}
		}

		const auto phenomenaMask = currentPathLength == 0 ? m_directPhenomenaMask : m_indirectPhenomenaMask;
		currentCtx.phenomena = SurfacePhenomena{baseContext.phenomena}.intersectWith(phenomenaMask);

		const math::Vector3R V = currentHit.getIncidentRay().getDir().mul(-1);

		BsdfSampleQuery bsdfSample{currentCtx};
		bsdfSample.inputs.set(currentHit, V);

		// Sample direct lighting
		if(currentPathLength + 1 == pathLength)
		{
			math::Spectrum Lo;
			if(directLight.bsdfSampleSurfacePathWithNee(bsdfSample, sampleFlow, &Lo))
			{
				if(out_Lo) { *out_Lo = pathThroughput * Lo; }
				return true;
			}
		}
		// Extend the path
		else
		{
			SurfaceHit nextHit;
			if(!surfaceTracer.bsdfSampleNextSurface(bsdfSample, sampleFlow, &nextHit) ||
			   !bsdfSample.outputs.isContributable())
			{
				return false;
			}

			currentHit = nextHit;
			pathThroughput *= bsdfSample.outputs.getPdfAppliedBsdfCos();

			// Prevent premature termination of the path due to solid angle compression/expansion
			rrScale /= bsdfSample.outputs.getRelativeIor2();
		}

		++currentPathLength;

		// Fixed hash target, so no additional randomness is introduced by this method
		currentCtx.key = currentCtx.key.getNext(0xD58379D1);
	}

	return false;
}

inline bool IndirectLightEstimator::bsdfSampleSurfacePathWithNee(
	const SurfaceHit&       X,
	const BsdfQueryContext& baseContext,
	SampleFlow&             sampleFlow,
	const std::size_t       minPathLength,
	const std::size_t       maxPathLength,
	math::Spectrum* const   out_Lo,
	const math::Spectrum&   initialPathWeight) const
{
	PH_ASSERT_GE(minPathLength, 1);
	PH_ASSERT_LE(minPathLength, maxPathLength);

	const SurfaceTracer surfaceTracer{m_scene};
	const DirectLightEstimator directLight{m_scene, baseContext.sidedness};

	SurfaceHit currentHit = X;
	BsdfQueryContext currentCtx = baseContext;
	math::Spectrum pathThroughput = initialPathWeight;
	math::Spectrum accuLo(0);
	real rrScale = 1.0_r;
	std::size_t currentPathLength = 0;
	while(currentPathLength < maxPathLength)// can extend by 1?
	{
		if(currentPathLength >= m_rrBeginPathLength)
		{
			real rrSurvivalProb;
			if(m_rr.surviveOnLuminance(pathThroughput * rrScale, sampleFlow, &rrSurvivalProb))
			{
				pathThroughput *= 1.0_r / rrSurvivalProb;
			}
			else
			{
				break;
			}
		}

		const auto phenomenaMask = currentPathLength == 0 ? m_directPhenomenaMask : m_indirectPhenomenaMask;
		currentCtx.phenomena = SurfacePhenomena{baseContext.phenomena}.intersectWith(phenomenaMask);

		const math::Vector3R V = currentHit.getIncidentRay().getDir().mul(-1);

		BsdfSampleQuery bsdfSample{currentCtx};
		bsdfSample.inputs.set(currentHit, V);

		// Account for energy from the specified path length range
		PH_ASSERT_LE(currentPathLength + 1, maxPathLength);
		if(currentPathLength + 1 >= minPathLength)
		{
			math::Spectrum Lo;
			std::optional<SurfaceHit> nextHit;
			if(!directLight.bsdfSampleSurfacePathWithNee(bsdfSample, sampleFlow, &Lo, &nextHit) ||
			   !bsdfSample.outputs.isContributable() ||
			   !nextHit)
			{
				break;
			}

			accuLo += pathThroughput * Lo;
			currentHit = *nextHit;
		}
		// Extend the path
		else
		{
			SurfaceHit nextHit;
			if(!surfaceTracer.bsdfSampleNextSurface(bsdfSample, sampleFlow, &nextHit) ||
			   !bsdfSample.outputs.isContributable())
			{
				break;
			}

			currentHit = nextHit;
		}

		pathThroughput *= bsdfSample.outputs.getPdfAppliedBsdfCos();

		// Prevent premature termination of the path due to solid angle compression/expansion
		rrScale /= bsdfSample.outputs.getRelativeIor2();

		++currentPathLength;

		// Fixed hash target, so no additional randomness is introduced by this method
		currentCtx.key = currentCtx.key.getNext(0xFFE10C53);
	}

	if(out_Lo) { *out_Lo = accuLo; }

	return true;
}

}// end namespace ph::lta
