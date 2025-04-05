#pragma once

#include "Engine/Core/Emitter/DiffuseSurfaceEmitterBase.h"
#include "Engine/Math/Color/Spectrum.h"
#include "Engine/Core/Texture/TTexture.h"

#include <Common/assertion.h>
#include <Common/primitive_type.h>

#include <memory>

namespace ph
{

class Primitive;

/*! @brief Diffusive surface emitter.
*/
class DiffuseSurfaceEmitter : public DiffuseSurfaceEmitterBase
{
public:
	DiffuseSurfaceEmitter(
		const Primitive* surface,
		const std::shared_ptr<TTexture<math::Spectrum>>& emittedEnergy,
		EmitterFeatureSet featureSet = defaultFeatureSet);

	void evalEmittedEnergy(const SurfaceHit& Xe, math::Spectrum* out_energy) const override;

	void genDirectSample(
		DirectEnergySampleQuery& query,
		SampleFlow& sampleFlow,
		HitProbe& probe) const override;

	void calcDirectPdf(DirectEnergyPdfQuery& query) const override;

	void emitRay(
		EnergyEmissionSampleQuery& query,
		SampleFlow& sampleFlow,
		HitProbe& probe) const override;

	real calcRadiantFluxApprox() const override;

	const Primitive& getSurface() const;
	const TTexture<math::Spectrum>& getEmittedEnergy() const;

private:
	const Primitive* m_surface;
	std::shared_ptr<TTexture<math::Spectrum>> m_emittedEnergy;
};

inline const Primitive& DiffuseSurfaceEmitter::getSurface() const
{
	PH_ASSERT(m_surface);
	return *m_surface;
}

inline const TTexture<math::Spectrum>& DiffuseSurfaceEmitter::getEmittedEnergy() const
{
	PH_ASSERT(m_emittedEnergy);
	return *m_emittedEnergy;
}

}// end namespace ph
