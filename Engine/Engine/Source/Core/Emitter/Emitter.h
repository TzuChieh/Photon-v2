#pragma once

#include "Math/math_fwd.h"
#include "Math/math.h"
#include "Math/Color/Spectrum.h"
#include "Utility/TBitFlags.h"

#include <Common/primitive_type.h>

namespace ph
{

class SurfaceHit;
class HitProbe;
class DirectEnergySampleQuery;
class DirectEnergyPdfQuery;
class EnergyEmissionSampleQuery;
class SampleFlow;

namespace detail
{

using EmitterFlagIntType = uint8;

inline constexpr auto efs_zero_bounce_sample_bits = math::flag_bit<uint8, 1>();
inline constexpr auto efs_bsdf_sample_bits        = math::flag_bit<uint8, 2>();
inline constexpr auto efs_direct_sample_bits      = math::flag_bit<uint8, 3>();
inline constexpr auto efs_emission_sample_bits    = math::flag_bit<uint8, 4>();

}// end namespace detail

/*! @brief Runtime features of an emitter.
@attention Some features are not handled by `Emitter` itself, as it is impossible to determine the
intended usage solely from its interface. Currently, light transport algorithms need to handle
`ZeroBounceSample` and `BsdfSample` by themselves.
*/
enum class EEmitterFeatureSet : detail::EmitterFlagIntType
{
	/*! Directly visualize the emitter. */
	ZeroBounceSample = detail::efs_zero_bounce_sample_bits,

	/*! Use BSDF sampling technique for rendering the emitter. */
	BsdfSample = detail::efs_bsdf_sample_bits,

	/*! Use direct sampling technique for rendering the emitter. */
	DirectSample = detail::efs_direct_sample_bits,

	/*! Use emission sampling technique for rendering the emitter. */
	EmissionSample = detail::efs_emission_sample_bits,

	// Special values:

	Default = 
		detail::efs_zero_bounce_sample_bits |
		detail::efs_bsdf_sample_bits |
		detail::efs_direct_sample_bits |
		detail::efs_emission_sample_bits,
};

PH_DEFINE_INLINE_ENUM_FLAG_OPERATORS(EEmitterFeatureSet);

using EmitterFeatureSet = TEnumFlags<EEmitterFeatureSet>;

/*! @brief An electromagnetic energy emitting source.
The emitted energy can be captured by a `Receiver`.
*/
class Emitter
{
public:
	constexpr static auto defaultFeatureSet = EmitterFeatureSet(EEmitterFeatureSet::Default);

	/*!
	@param featureSet Request a set of features for this emitter.
	*/
	explicit Emitter(EmitterFeatureSet featureSet = defaultFeatureSet);

	virtual ~Emitter();

	/*! @brief Evaluate emitted energy from a point on the surface.
	@param Xe The point that emits energy.
	*/
	virtual void evalEmittedEnergy(const SurfaceHit& Xe, math::Spectrum* out_energy) const = 0;

	/*! @brief Sample direct lighting for a target position.
	@note
	- Generates hit event (with `DirectEnergySampleOutput::getObservationRay()` and `probe`)
	- Handles `EEmitterFeatureSet::DirectSample`
	*/
	virtual void genDirectSample(
		DirectEnergySampleQuery& query, 
		SampleFlow& sampleFlow,
		HitProbe& probe) const = 0;

	/*! @brief Calculate the PDF of direct lighting for a target position.
	@note
	- Handles `EEmitterFeatureSet::DirectSample`
	*/
	virtual void calcDirectPdf(DirectEnergyPdfQuery& query) const = 0;

	/*! @brief Emit a ray that carries some amount of energy from this emitter.
	@note
	- Generates hit event (with `EnergyEmissionSampleOutput::getEmittedRay()` and `probe`)
	- Handles `EEmitterFeatureSet::EmissionSample`
	*/
	virtual void emitRay(
		EnergyEmissionSampleQuery& query, 
		SampleFlow& sampleFlow,
		HitProbe& probe) const = 0;

	virtual real calcRadiantFluxApprox() const;

	/*!
	@return Features requested for this emitter.
	*/
	EmitterFeatureSet getFeatureSet() const;

private:
	EmitterFeatureSet m_featureSet;
};

// In-header Implementations:

inline real Emitter::calcRadiantFluxApprox() const
{
	// Non-zero to avoid not being sampled
	return 1.0_r;
}

inline EmitterFeatureSet Emitter::getFeatureSet() const
{
	return m_featureSet;
}

}// end namespace ph
