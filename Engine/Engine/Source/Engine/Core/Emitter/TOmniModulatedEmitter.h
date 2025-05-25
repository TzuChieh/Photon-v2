#pragma once

#include "Engine/Core/Emitter/SurfaceEmitter.h"
#include "Engine/Core/Emitter/VolumeEmitter.h"
#include "Engine/Core/Texture/TTexture.h"
#include "Engine/Core/Intersection/UvwMapper/SphericalMapper.h"

#include <memory>
#include <type_traits>

namespace ph
{

template<typename SourceEmitter>
class TOmniModulatedEmitter : public SourceEmitter
{
	static_assert(
		std::is_base_of_v<SurfaceEmitter, SourceEmitter> ||
		std::is_base_of_v<VolumeEmitter, SourceEmitter>);

public:
	/*! @brief Given a source, construct its modulated version.
	Feature set is inherited from `source`. If you want to specify a specific feature set, use
	`OmniModulatedEmitter(const SourceEmitter*, EmitterFeatureSet)`.
	*/
	explicit TOmniModulatedEmitter(const SourceEmitter* source);

	TOmniModulatedEmitter(
		const SourceEmitter* source,
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
	const SourceEmitter& getSource() const;

private:
	const SourceEmitter*                      m_source;
	std::shared_ptr<TTexture<math::Spectrum>> m_filter;
	SphericalMapper                           m_dirToUv;
};

}// end namespace ph

#include "Engine/Core/Emitter/TOmniModulatedEmitter.ipp"
