#pragma once

#include "Common/primitive_type.h"
#include "Math/math_fwd.h"

#include <memory>

namespace ph
{

class SurfaceHit;
class ScatterFunction;
class BlockFunction;
class EmitFunction;

class VolumeOptics
{
public:
	virtual ~VolumeOptics() = 0;

	// TODO: sample

private:

	// TODO: categorize into absorption-only volume or other common types

	std::shared_ptr<ScatterFunction> m_scatterFunc;
	std::shared_ptr<BlockFunction> m_blockFunc;
	std::shared_ptr<EmitFunction> m_emitFunc;
};

}// end namespace ph