#pragma once

#include "RenderCore/ghi_fwd.h"

#include <Common/primitive_type.h>
#include <Math/Geometry/TAABB2D.h>

#include <cstddef>
#include <vector>

namespace ph::editor::render
{

/*! @brief Information about the rendered frame.
For frame data exchange between the renderer and outside world.
*/
class OfflineRenderPeek final
{
public:
	struct Input final
	{
		/*! Index of the target frame layer for retrieving data.
		*/
		std::size_t layerIndex = 0;

		ghi::TextureHandle resultHandle;

		bool wantIntermediateResult = false;
		bool wantUpdatingRegions = false;
		bool performToneMapping = true;
	};

	struct Output final
	{
		/*! Areas in the frame that are still being updated.
		*/
		std::vector<math::TAABB2D<int32>> updatingRegions;
	};

	Input in;
	Output out;
};

}// end namespace ph::editor::render
