#pragma once

#include "Engine/Math/math_fwd.h"
#include "Engine/Math/Color/Spectrum.h"

#include <Common/primitive_type.h>

#include <memory>

namespace ph
{

class SurfaceHit;
class PhaseFunction;
class EmissionFunction;
class MediumDistanceSample;

class VolumeOptics
{
public:
	virtual ~VolumeOptics() = 0;

	void genDistanceSample(MediumDistanceSample& sample) const;

private:
	virtual void genDistanceSample(
		const SurfaceHit& X, 
		const math::Vector3R& L,
		real maxDist, 
		real* out_dist, 
		math::Spectrum* out_pdfAppliedWeight) const = 0;

	//std::shared_ptr<ScatterFunction> m_scatterFunc;
	//std::shared_ptr<BlockFunction> m_blockFunc;
	//std::shared_ptr<EmitFunction> m_emitFunc;
};

}// end namespace ph
