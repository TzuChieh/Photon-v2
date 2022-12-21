#pragma once

#include <Common/primitive_type.h>

namespace ph::editor
{

class GHIInfoDeviceCapability final
{
public:
	uint8 maxTextureUnitsForVertexShadingStage : 6 = 0;
	uint8 maxTextureUnitsForFragmentShadingStage : 6 = 0;

	/*!  
	Note that the maximum number of elements/components of a single attribute is generally 4.
	A 4x4 matrix would normally occupy 4 attributes.
	*/
	uint8 maxVertexAttributes : 5 = 0;
};

}// end namespace ph::editor
