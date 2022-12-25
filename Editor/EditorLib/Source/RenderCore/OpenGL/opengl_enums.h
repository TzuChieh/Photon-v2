#pragma once

#include "ThirdParty/glad2.h"
#include "RenderCore/ghi_enums.h"

#include <Common/primitive_type.h>

namespace ph::editor::opengl
{

GLenum to_internal_format(EGHIInfoPixelFormat format);
GLenum to_color_attachment(uint32 slotIndex);
GLenum to_base_format(GLenum internalFormat);
uint32 num_pixel_components(GLenum internalFormat);
bool is_color_format(GLenum internalFormat);

GLenum translate(EGHIInfoPixelComponent componentType);
GLenum translate(EGHIInfoStorageElement elementType);
GLenum translate(EGHIInfoMeshDrawMode drawMode);
GLenum translate(EGHIInfoShadingStage shadingStage);

}// end namespace ph::editor::opengl
