#pragma once

#include "Core/Emitter/Emitter.h"
#include "Core/Texture/TTexture.h"
#include "Core/Intersection/UvwMapper/SphericalMapper.h"

#include <Common/assertion.h>
#include <Common/primitive_type.h>

#include <memory>

namespace ph
{

class OmniModulatedEmitter : public Emitter
{
public:
	/*! @brief Given a source, construct its modulated version.
	Feature set is inherited from `source`. If you want to specify a specific feature set, use
	`OmniModulatedEmitter(const Emitter*, EmitterFeatureSet)`.
	*/
	explicit OmniModulatedEmitter(const Emitter* source);

	OmniModulatedEmitter(
		const Emitter*    source,
		EmitterFeatureSet featureSet);

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

	void setFilter(const std::shared_ptr<TTexture<math::Spectrum>>& filter);

	/*!
	@return The emitter that is being modulated.
	*/
	const Emitter& getSource() const;

private:
	const Emitter*                            m_source;
	std::shared_ptr<TTexture<math::Spectrum>> m_filter;
	SphericalMapper                           m_dirToUv;
};

inline const Emitter& OmniModulatedEmitter::getSource() const
{
	PH_ASSERT(m_source);
	return *m_source;
}

}// end namespace ph
