#pragma once

#include "Core/Emitter/DiffuseSurfaceEmitterBase.h"
#include "Math/Color/Spectrum.h"
#include "Core/Texture/TTexture.h"
#include "Utility/TSpan.h"

#include <Common/assertion.h>

#include <cstddef>
#include <vector>
#include <memory>

namespace ph
{

class Primitive;

/*! @brief Diffusive surface emitter comprised of multiple surface fragments.
All surface fragments use the same energy texture.
*/
class GroupedDiffuseSurfaceEmitter : public DiffuseSurfaceEmitterBase
{
public:
	explicit GroupedDiffuseSurfaceEmitter(
		TSpanView<const Primitive*> surfaces,
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

	/*!
	@return Number of surface fragments.
	*/
	std::size_t numSurfaces() const;

	/*! @brief Get a surface fragment by index.
	*/
	const Primitive& getSurface(std::size_t surfaceIdx) const;

	const TTexture<math::Spectrum>& getEmittedEnergy() const;

private:
	std::vector<const Primitive*> m_surfaces;
	std::shared_ptr<TTexture<math::Spectrum>> m_emittedEnergy;
};

inline std::size_t GroupedDiffuseSurfaceEmitter::numSurfaces() const
{
	return m_surfaces.size();
}

inline const Primitive& GroupedDiffuseSurfaceEmitter::getSurface(
	const std::size_t surfaceIdx) const
{
	PH_ASSERT_LT(surfaceIdx, m_surfaces.size());
	return *m_surfaces[surfaceIdx];
}

inline const TTexture<math::Spectrum>& GroupedDiffuseSurfaceEmitter::getEmittedEnergy() const
{
	PH_ASSERT(m_emittedEnergy);
	return *m_emittedEnergy;
}

}// end namespace ph
