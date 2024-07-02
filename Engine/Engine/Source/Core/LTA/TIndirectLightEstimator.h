#pragma once

#include "Math/math_fwd.h"
#include "Math/Color/Spectrum.h"
#include "Core/LTA/SidednessAgreement.h"

#include <cstddef>

namespace ph { class Scene; }
namespace ph { class SurfaceHit; }
namespace ph { class SampleFlow; }

namespace ph::lta
{

class RussianRoulette;

/*! @brief Estimate indirect lighting for a surface point.
@tparam POLICY The default sidedness policy to use if none can be inferred from the inputs.
*/
template<ESidednessPolicy POLICY>
class TIndirectLightEstimator final
{
public:
	explicit TIndirectLightEstimator(const Scene* scene);

	/*! @brief Constructing path with BSDF sampling and estimate lighting with both BSDF sampling and next-event estimation.
	The estimated lighting is for the specific `pathLength`. This method is effectively the same as
	`TDirectLightEstimator::bsdfSamplePathWithNee()` if input path length is 1.
	@param pathLength The length of the path to construct.
	@param out_Lo Sampled outgoing energy from `X`. The energy is for the specified path length only.
	@param rrBeginPathLength When to start using russian roulette. If 0, russian roulette will be
	performed right away, before any sampling take place.
	@return Whether output parameters are usable. If `false` is returned, the sample should still
	be treated as valid, albeit its contribution is effectively zero.
	*/
	[[nodiscard]]
	bool bsdfSamplePathWithNee(
		const SurfaceHit&      X,
		SampleFlow&            sampleFlow,
		std::size_t            pathLength,
		const RussianRoulette& rr,
		math::Spectrum*        out_Lo = nullptr,
		std::size_t            rrBeginPathLength = 0,
		const math::Spectrum&  initialPathWeight = math::Spectrum(1)) const;

	/*! @brief Constructing path with BSDF sampling and estimate lighting with both BSDF sampling and next-event estimation.
	The estimated lighting is for the specific path length range [minPathLength, maxPathLength].
	This method is effectively the same as `TDirectLightEstimator::bsdfSamplePathWithNee()` if
	input path length is in range [1, 1].
	@param minPathLength The minimum length of the path to construct.
	@param maxPathLength The maximum length of the path to construct (inclusive).
	@param out_Lo Sampled outgoing energy from `X`. The energy is for the specified path length range only.
	@param rrBeginPathLength When to start using russian roulette. If 0, russian roulette will be
	performed right away, before any sampling take place.
	@return Whether output parameters are usable. If `false` is returned, the sample should still
	be treated as valid, albeit its contribution is effectively zero.
	*/
	[[nodiscard]]
	bool bsdfSamplePathWithNee(
		const SurfaceHit&      X,
		SampleFlow&            sampleFlow,
		std::size_t            minPathLength,
		std::size_t            maxPathLength,
		const RussianRoulette& rr,
		math::Spectrum*        out_Lo = nullptr,
		std::size_t            rrBeginPathLength = 0, 
		const math::Spectrum&  initialPathWeight = math::Spectrum(1)) const;

private:
	const Scene* m_scene;
};

}// end namespace ph::lta

#include "Core/LTA/TIndirectLightEstimator.ipp"
