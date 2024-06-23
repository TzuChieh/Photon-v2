#pragma once

#include "Core/Emitter/SurfaceEmitter.h"
#include "Core/Emitter/DiffuseSurfaceEmitter.h"
#include "Math/Color/Spectrum.h"
#include "Core/Texture/TTexture.h"
#include "Utility/TSpan.h"

#include <Common/assertion.h>

#include <cstddef>
#include <vector>
#include <memory>

namespace ph
{

class MultiDiffuseSurfaceEmitter : public SurfaceEmitter
{
public:
	explicit MultiDiffuseSurfaceEmitter(
		TSpanView<DiffuseSurfaceEmitter> emitters,
		EmitterFeatureSet featureSet = defaultFeatureSet);

	void evalEmittedEnergy(const SurfaceHit& X, math::Spectrum* out_energy) const override;

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

	/*!
	@return Number of internal emitters.
	*/
	std::size_t numEmitters() const;

	/*! @brief Add a new internal emitter.
	@return The newly added emitter (constructed from the input surface).
	@warning The returned emitter address is not stable while new emitters are still being added.
	*/
	DiffuseSurfaceEmitter& addEmitter(const DiffuseSurfaceEmitter& emitter);

	/*! @brief Get the internal emitter by index.
	@warning The returned emitter address is not stable while new emitters are still being added.
	*/
	///@{
	DiffuseSurfaceEmitter& getEmitter(std::size_t emitterIdx);
	const DiffuseSurfaceEmitter& getEmitter(std::size_t emitterIdx) const;
	///@}

private:
	/*! Internal emitters.
	*/
	std::vector<DiffuseSurfaceEmitter> m_emitters;

	/*! Mapping external primitives to internal emitters. We need this as internal emitters are not
	registered to cooked data storage and cannot be obtained from input primitives. 
	*/
	std::unordered_map<const Primitive*, std::size_t> m_primitiveToEmitterIdx;
};

inline std::size_t MultiDiffuseSurfaceEmitter::numEmitters() const
{
	return m_emitters.size();
}

inline DiffuseSurfaceEmitter& MultiDiffuseSurfaceEmitter::getEmitter(
	const std::size_t emitterIdx)
{
	PH_ASSERT_LT(emitterIdx, m_emitters.size());

	return m_emitters[emitterIdx];
}

inline const DiffuseSurfaceEmitter& MultiDiffuseSurfaceEmitter::getEmitter(
	const std::size_t emitterIdx) const
{
	PH_ASSERT_LT(emitterIdx, m_emitters.size());

	return m_emitters[emitterIdx];
}

}// end namespace ph
