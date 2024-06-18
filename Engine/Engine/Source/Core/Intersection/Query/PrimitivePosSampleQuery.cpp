#include "Core/Intersection/Query/PrimitivePosSampleQuery.h"
#include "Core/Emitter/Query/DirectEnergySampleQuery.h"

namespace ph
{

void PrimitivePosSampleInput::set(
	const DirectEnergySampleInput& directInput,
	const math::Vector3R& uvw,
	const lta::PDF& uvwPdf,
	const bool suggestDir)
{
	set(
		directInput.getX().getTime(),
		directInput.getTargetPos(),
		uvw,
		uvwPdf,
		suggestDir);
}

}// end namespace ph
