#pragma once

#include <Common/primitive_type.h>

namespace ph::editor
{

class GHIInfoDeviceCapability final
{
public:
	uint32 maxTextureUnitsForVertexShadingStage : 6 = 0;
	uint32 maxTextureUnitsForFragmentShadingStage : 6 = 0;
	uint32 maxVertexAttributes : 5 = 0;
};

}// end namespace ph::editor
