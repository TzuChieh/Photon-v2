#pragma once

#include <Common/primitive_type.h>

namespace ph
{

/*! @brief Common parameters of path tracing.
*/
struct PTEstimatorParams
{
	inline static constexpr uint32 DEFAULT_MAX_PATH_LENGTH = 16384;

	/*! Maximum path length. For example, 1 will show only emissive surfaces; 2 will incorporate
	direct lighting, 3 will incorporate 1-bounce indirect lighting, etc. The default value is
	effectively infinite bounces.
	*/
	uint32 maxPathLength = DEFAULT_MAX_PATH_LENGTH;

	/*! Hint for the path length to start using Russian roulette to randomly terminate the path.
	*/
	uint32 rrBeginLengthHint = 4;

	/*! Whether volumetric effects should be handled.
	*/
	bool includeVolumetricEffects = false;
};

}// end namespace ph
