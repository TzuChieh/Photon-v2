#pragma once

#include "Common/primitive_type.h"
#include "Math/math_fwd.h"
#include "Core/Quantity/SpectralStrength.h"

#include <memory>

namespace ph
{

class SurfaceHit;
class ScatterFunction;
class BlockFunction;
class EmitFunction;
class VolumeDistanceSample;

class VolumeOptics
{
public:
	virtual ~VolumeOptics() = 0;

	void sample(VolumeDistanceSample& sample) const;

private:
	virtual void sampleDistance(
		const SurfaceHit& X, 
		const Vector3R& L, 
		real maxDist, 
		real* out_dist, 
		SpectralStrength* out_pdfAppliedWeight) const = 0;

	//std::shared_ptr<ScatterFunction> m_scatterFunc;
	//std::shared_ptr<BlockFunction> m_blockFunc;
	//std::shared_ptr<EmitFunction> m_emitFunc;
};

}// end namespace ph