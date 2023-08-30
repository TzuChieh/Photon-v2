#pragma once

#include "RenderCore/ghi_enums.h"

#include "ThirdParty/glad2.h"

#include <Common/primitive_type.h>

namespace ph::editor::opengl
{

GLenum to_internal_format(EGHIPixelFormat format);
GLenum to_color_attachment(uint32 slotIndex);
GLenum to_base_format(GLenum internalFormat);
uint32 num_pixel_components(GLenum internalFormat);
bool is_color_format(GLenum internalFormat);

GLenum translate(EGHIPixelComponent componentType);
GLenum translate(EGHIStorageElement elementType);
GLenum translate(EGHIMeshDrawMode drawMode);
GLenum translate(EGHIShadingStage shadingStage);

}// end namespace ph::editor::opengl
