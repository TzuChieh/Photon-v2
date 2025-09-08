#pragma once

#include "Engine/Math/math_fwd.h"
#include "Engine/Math/Color/Spectrum.h"
#include "Engine/Core/LTA/SidednessAgreement.h"
#include "Engine/Core/LTA/RussianRoulette.h"
#include "Engine/Core/SurfaceBehavior/surface_optics_fwd.h"

#include <cstddef>

namespace ph { class Scene; }
namespace ph { class SurfaceHit; }
namespace ph { class SampleFlow; }
namespace ph { class BsdfQueryContext; }

namespace ph::lta
{

class RussianRoulette;

/*! @brief Estimate indirect lighting for a surface point.
*/
class IndirectLightEstimator final
{
public:
	/*!
	@param rrBeginPathLength When to start using russian roulette. If 0, russian roulette will be
	performed right away, before any sampling take place.
	*/
	explicit IndirectLightEstimator(
		const Scene* scene,
		const SurfacePhenomena& directPhenomenaMask = ALL_SURFACE_PHENOMENA,
		const SurfacePhenomena& indirectPhenomenaMask = ALL_SURFACE_PHENOMENA,
		const RussianRoulette& rr = RussianRoulette{},
		std::size_t rrBeginPathLength = 0);

	/*! @brief Constructing surface path with BSDF sampling and estimate lighting with both BSDF sampling and next-event estimation.
	The estimated lighting is for the specific `pathLength`. This method is effectively the same as
	`DirectLightEstimator::bsdfSampleSurfacePathWithNee()` if input path length is 1.
	@param baseContext Base of the context for BSDF query in direct and indirect lighting. It may
	be updated by options like phenomena masks.
	@param pathLength The length of the path to construct.
	@param out_Lo Sampled outgoing energy from `X`. The energy is for the specified path length only.
	@return Whether output parameters are usable. If `false` is returned, the sample should still
	be treated as valid, albeit its contribution is effectively zero.
	*/
	[[nodiscard]]
	bool bsdfSampleSurfacePathWithNee(
		const SurfaceHit&       X,
		const BsdfQueryContext& baseContext,
		SampleFlow&             sampleFlow,
		std::size_t             pathLength,
		math::Spectrum*         out_Lo = nullptr,
		const math::Spectrum&   initialPathWeight = math::Spectrum(1)) const;

	/*! @brief Constructing path with BSDF sampling and estimate lighting with both BSDF sampling and next-event estimation.
	The estimated lighting is for the specific path length range [minPathLength, maxPathLength].
	This method is effectively the same as `DirectLightEstimator::bsdfSampleSurfacePathWithNee()` if
	input path length is in range [1, 1].
	@param baseContext Base of the context for BSDF query in direct and indirect lighting. It may
	be updated by options like phenomena masks.
	@param minPathLength The minimum length of the path to construct.
	@param maxPathLength The maximum length of the path to construct (inclusive).
	@param out_Lo Sampled outgoing energy from `X`. The energy is for the specified path length range only.
	@return Whether output parameters are usable. If `false` is returned, the sample should still
	be treated as valid, albeit its contribution is effectively zero.
	*/
	[[nodiscard]]
	bool bsdfSampleSurfacePathWithNee(
		const SurfaceHit&       X,
		const BsdfQueryContext& baseContext,
		SampleFlow&             sampleFlow,
		std::size_t             minPathLength,
		std::size_t             maxPathLength,
		math::Spectrum*         out_Lo = nullptr,
		const math::Spectrum&   initialPathWeight = math::Spectrum(1)) const;

private:
	const Scene* m_scene;
	RussianRoulette m_rr;
	std::size_t m_rrBeginPathLength;
	SurfacePhenomena m_directPhenomenaMask;
	SurfacePhenomena m_indirectPhenomenaMask;
};

}// end namespace ph::lta

#include "Engine/Core/LTA/IndirectLightEstimator.ipp"
