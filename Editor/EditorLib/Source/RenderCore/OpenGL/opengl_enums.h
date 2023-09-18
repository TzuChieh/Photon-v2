#pragma once

#include "RenderCore/ghi_enums.h"

#include "ThirdParty/glad2.h"

#include <Common/primitive_type.h>

namespace ph::editor::opengl
{

GLenum to_internal_format(EGHISizedPixelFormat sizedFormat);
GLenum to_internal_format(EGHIPixelFormat unsizedFormat);
GLenum to_color_attachment(uint32 slotIndex);
GLenum to_data_type(EGHIPixelComponent componentType);
GLenum to_data_type(EGHIStorageElement elementType);
GLenum to_primitive_type(EGHIMeshDrawMode drawMode);
GLenum to_shader_type(EGHIShadingStage shadingStage);
GLenum to_filter_type(EGHIFilterMode filterMode);
GLenum to_wrap_type(EGHIWrapMode wrapMode);

GLenum to_base_format(GLenum internalFormat);
uint32 num_pixel_components(GLenum internalFormat);
bool is_color_format(GLenum internalFormat);

}// end namespace ph::editor::opengl
