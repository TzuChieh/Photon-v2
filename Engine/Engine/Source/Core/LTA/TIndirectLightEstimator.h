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
*/
template<ESidednessPolicy POLICY>
class TIndirectLightEstimator final
{
public:
	explicit TIndirectLightEstimator(const Scene* scene);

	/*! @brief Constructing path with BSDF sampling and estimate lighting with both BSDF sampling and next-event estimation.
	@param pathLength The length of the path to construct. This method is effectively the same as
	`TDirectLightEstimator::bsdfSampleOutgoingWithNee()` if input is 1.
	@param out_Lo Sampled energy. The energy is for the specified path length only.
	@return Whether output parameters are usable. If `false` is returned, the sample should still
	be treated as valid, albeit its contribution is effectively zero.
	*/
	bool bsdfSamplePathWithNee(
		const SurfaceHit&      X,
		SampleFlow&            sampleFlow,
		std::size_t            pathLength,
		const RussianRoulette& rr,
		math::Spectrum*        out_Lo = nullptr) const;

private:
	const Scene* m_scene;
};

}// end namespace ph::lta

#include "Core/LTA/TIndirectLightEstimator.ipp"
