#pragma once

#include "EditorCore/Storage/TWeakHandle.h"

#include <Common/primitive_type.h>

namespace ph::editor::render
{

class Texture;

/*! @brief Handle for render resource management.
*/
///@{
using TextureHandle = TWeakHandle<Texture, uint32, uint32>;
///@}

}// end namespace ph::editor::render
