#pragma once

#include "Engine/Math/math_fwd.h"
#include "Engine/Math/Color/Spectrum.h"
#include "Engine/Core/LTA/SidednessAgreement.h"
#include "Engine/Core/Emitter/Emitter.h"

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
namespace ph { class Ray; }

namespace ph::lta
{

/*! @brief Estimate direct lighting for a surface point.
This is a lightweight helper type for estimating direct lighting. Do not think "direct light" as
lighting from a directional light source, it means the first-bounce lighting for any surface point,
and the surface point can be the N-th one in a path.
*/
class DirectLightEstimator final
{
public:
	/*!
	@param defaultSidedness Default sidedness agreement to use when none is specified.
	*/
	explicit DirectLightEstimator(
		const Scene* scene,
		const SidednessAgreement& defaultSidedness = SidednessAgreement{ESidednessPolicy::Strict});

	/*! @brief Find the next physical surface and accumulate emission along the ray.
	Emission is returned even when no physical surface is found, without MIS or throughput weighting.
	@tparam FEATURE Allowed emitter features.
	@param out_X The next physical surface. Valid only when true is returned.
	@param out_Le Accumulated emission. Always set.
	@return Whether the next physical surface agrees with the estimator's sidedness policy.
	*/
	template<EEmitterFeatureSet FEATURE = EEmitterFeatureSet::Default>
	bool sampleSurfaceEmission(
		const Ray& ray,
		SampleFlow& sampleFlow,
		SurfaceHit* out_X,
		math::Spectrum* out_Le) const;

	/*! @brief Sample surface lighting using BSDF's suggestion.
	A light sampling technique that is always usable.
	@param bsdfSample BSDF sample result. Validity of its output should be explicitly tested before use.
	@param out_Le Emission along the sampled segment. Does not contain any weighting.
	@param out_X The next physical surface, if any. Emission can be nonzero without a physical hit.
	@return Whether output parameters are usable. If `false` is returned, the sample should still
	be treated as valid, albeit its contribution is effectively zero.
	*/
	[[nodiscard]]
	bool bsdfSampleSurfaceEmission(
		BsdfSampleQuery&           bsdfSample,
		SampleFlow&                sampleFlow,
		math::Spectrum*            out_Le = nullptr,
		std::optional<SurfaceHit>* out_X = nullptr) const;

	/*! @brief Sample surface lighting using next-event estimation.
	This light sampling technique may not always be usable. Calling this method when `isNeeSamplable()`
	returns `false` is an error.
	@param directSample Direct energy sample result. Validity of its output should be explicitly tested
	before use.
	@param out_Xe Returns the surface that is sampled. It is always an energy-emitting surface.
	@return Whether output parameters are usable. If `false` is returned, the sample should still
	be treated as valid, albeit its contribution is effectively zero.
	*/
	[[nodiscard]]
	bool neeSampleSurfaceEmission(
		DirectEnergySampleQuery&   directSample,
		SampleFlow&                sampleFlow,
		SurfaceHit*                out_Xe = nullptr) const;

	/*! @brief Sample surface lighting by combining the techniques used by `bsdfSampleSurfaceEmission()` and `neeSampleSurfaceEmission()`.
	A light sampling technique that is always usable.
	@param bsdfSample BSDF sample result. Validity of its output should be explicitly tested before use.
	@param out_Lo The sampled outgoing energy from `X`. The sample is properly weighted with any
	required BSDFs and PDFs.
	@param out_X The next physical surface, if any. Emission can be nonzero without a physical hit.
	@param nonBlockingSampleProbability Probability of traversing nonblocking lights, in [0, 1].
	0 disables nonblocking emission accumulation along the BSDF-sampled segment.
	@return Whether output parameters are usable. If `false` is returned, the sample should still
	be treated as valid, albeit its contribution is effectively zero.
	*/
	[[nodiscard]]
	bool bsdfSampleSurfacePathWithNee(
		BsdfSampleQuery&           bsdfSample,
		SampleFlow&                sampleFlow,
		math::Spectrum*            out_Lo = nullptr,
		std::optional<SurfaceHit>* out_X = nullptr,
		real                       nonBlockingSampleProbability = 1) const;

	/*! @brief Sum contributions from nonblocking hits and an optional physical endpoint.
	@tparam FEATURE Allowed features; an emitter must enable at least one.
	@param ray Segment already bounded by the nearest physical surface, if any.
	@param endpoint Physical hit agreeing with sidedness, or `nullptr`.
	@param sampleFlow Used for roulette only when `nonBlockingSampleProbability` is in (0, 1).
	@param energyFunc Called as `energyFunc(const SurfaceHit& emitterHit) -> math::Spectrum`.
	Returns each hit's complete contribution, including emission and any weighting or clamping.
	@param nonBlockingSampleProbability Survival probability in [0, 1]. 0 skips nonblocking hits;
	otherwise surviving contributions are divided by this probability. The endpoint is always processed.
	*/
	template<EEmitterFeatureSet FEATURE = EEmitterFeatureSet::Default, typename EnergyFunc>
	math::Spectrum accumulateSurfaceEmission(
		const Ray& ray,
		const SurfaceHit* endpoint,
		SampleFlow& sampleFlow,
		EnergyFunc&& energyFunc,
		real nonBlockingSampleProbability = 1) const;

	/*! @brief Get the solid angle domain PDF of an next-event estimation lighting sample.
	Surface occlusion is not taken into account. Calling this method when `isNeeSamplable()`
	returns `false` is an error.
	*/
	[[nodiscard]]
	real calcNeePdfWUnoccluded(
		const SurfaceHit&          X,
		const SurfaceHit&          Xe) const;

	/*!
	@return Checks whether next-event estimation is a valid technique to use on the surface `X`.
	@note Depends on the surface optical properties only. Does not check the emitter feature set.
	*/
	[[nodiscard]]
	bool isNeeSamplable(const SurfaceHit& X) const;

private:
	const Scene& getScene() const;

	const Scene* m_scene;
	SidednessAgreement m_defaultSidedness;
};

}// end namespace ph::lta

#include "Engine/Core/LTA/DirectLightEstimator.ipp"
