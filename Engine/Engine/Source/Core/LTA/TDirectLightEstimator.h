#pragma once

#include "Math/math_fwd.h"
#include "Math/Color/Spectrum.h"
#include "Core/LTA/SidednessAgreement.h"

#include <Common/primitive_type.h>

#include <optional>

namespace ph { class Scene; }
namespace ph { class SurfaceHit; }
namespace ph { class SampleFlow; }
namespace ph { class Emitter; }
namespace ph { class Primitive; }
namespace ph { class SurfaceOptics; }
namespace ph { class BsdfSampleQuery; }
namespace ph { class DirectEnergySampleQuery; }

namespace ph::lta
{

/*! @brief Estimate direct lighting for a surface point.
This is a lightweight helper type for estimating direct lighting. Do not think "direct light" as
lighting from a directional light source, it means the first-bounce lighting for any surface point,
and the surface point can be the N-th one in a path.
@tparam POLICY The default sidedness policy to use if none can be inferred from the inputs.
*/
template<ESidednessPolicy POLICY>
class TDirectLightEstimator final
{
public:
	explicit TDirectLightEstimator(const Scene* scene);

	/*! @brief Sample lighting using BSDF's suggestion.
	A light sampling technique that is always valid.
	@param bsdfSample BSDF sample result. Validity of its output should be explicitly tested before use.
	@param out_Le The sampled emitted energy of another surface. Does not contain any weighting.
	@param out_X Returns the surface that is sampled. It is not necessary an energy-emitting
	surface (as `out_Le` could be 0 if `out_X` is requested).
	@return Whether output parameters are usable. If `false` is returned, the sample should still
	be treated as valid, albeit its contribution is effectively zero.
	*/
	[[nodiscard]]
	bool bsdfSampleEmission(
		BsdfSampleQuery&           bsdfSample,
		SampleFlow&                sampleFlow,
		math::Spectrum*            out_Le = nullptr,
		std::optional<SurfaceHit>* out_X = nullptr) const;

	/*! @brief Sample lighting using next-event estimation.
	This light sampling technique may not always be valid. Calling this method when `isNeeSamplable()`
	returns `false` is an error.
	@param directSample Direct energy sample result. Validity of its output should be explicitly tested
	before use.
	@param out_Xe Returns the surface that is sampled. It is always an energy-emitting surface.
	@return Whether output parameters are usable. If `false` is returned, the sample should still
	be treated as valid, albeit its contribution is effectively zero.
	*/
	[[nodiscard]]
	bool neeSampleEmission(
		DirectEnergySampleQuery&   directSample,
		SampleFlow&                sampleFlow,
		SurfaceHit*                out_Xe = nullptr) const;

	/*! @brief Sample lighting by combining the techniques used by `bsdfSample()` and `neeSample()`.
	A light sampling technique that is always valid.
	@param bsdfSample BSDF sample result. Validity of its output should be explicitly tested before use.
	@param out_Lo The sampled outgoing energy from `X`. The sample is properly weighted with any
	required BSDFs and PDFs.
	@param out_X Returns the surface that is sampled. It is not necessary an energy-emitting
	surface (as `out_Le` could be 0 if `out_X` is requested).
	@return Whether output parameters are usable. If `false` is returned, the sample should still
	be treated as valid, albeit its contribution is effectively zero.
	*/
	[[nodiscard]]
	bool bsdfSamplePathWithNee(
		BsdfSampleQuery&           bsdfSample,
		SampleFlow&                sampleFlow,
		math::Spectrum*            out_Lo = nullptr,
		std::optional<SurfaceHit>* out_X = nullptr) const;

	/*! @brief Get the solid angle domain PDF of an next-event estimation lighting sample.
	Surface occlusion is not taken into account. Calling this method when `isNeeSamplable()`
	returns `false` is an error.
	*/
	[[nodiscard]]
	real neeSamplePdfWUnoccluded(
		const SurfaceHit&          X,
		const SurfaceHit&          Xe) const;

	/*!
	@return Checks whether next-event estimation is a valid technique to use on the surface `X`.
	*/
	[[nodiscard]]
	bool isNeeSamplable(const SurfaceHit& X) const;

private:
	const Scene& getScene() const;

	const Scene* m_scene;
};

}// end namespace ph::lta

#include "Core/LTA/TDirectLightEstimator.ipp"
