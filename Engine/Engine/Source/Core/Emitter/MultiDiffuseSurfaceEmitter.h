#pragma once

#include "Core/Emitter/SurfaceEmitter.h"
#include "Core/Emitter/DiffuseSurfaceEmitter.h"

#include <vector>

namespace ph
{

class MultiDiffuseSurfaceEmitter : public SurfaceEmitter
{
public:
	explicit MultiDiffuseSurfaceEmitter(const std::vector<DiffuseSurfaceEmitter>& emitters);

	void evalEmittedRadiance(const SurfaceHit& X, math::Spectrum* out_radiance) const override;

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

	void setFrontFaceEmit() override;
	void setBackFaceEmit() override;

	void addEmitter(const DiffuseSurfaceEmitter& emitter);
	void setEmittedRadiance(const std::shared_ptr<TTexture<math::Spectrum>>& emittedRadiance);
	const TTexture<math::Spectrum>& getEmittedRadiance() const;

private:
	std::vector<DiffuseSurfaceEmitter> m_emitters;
	real                               m_extendedArea;
	real                               m_reciExtendedArea;
};

}// end namespace ph
