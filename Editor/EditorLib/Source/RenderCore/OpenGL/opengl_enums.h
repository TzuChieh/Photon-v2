#pragma once

#include "RenderCore/ghi_enums.h"

#include "ThirdParty/glad2.h"

#include <Common/primitive_type.h>

namespace ph::editor::ghi::opengl
{

GLenum to_internal_format(ESizedPixelFormat sizedFormat);
GLenum to_internal_format(EPixelFormat unsizedFormat);
GLenum to_color_attachment(uint32 slotIndex);
GLenum to_data_type(EPixelComponent componentType);
GLenum to_data_type(EStorageElement elementType);
GLenum to_primitive_type(EMeshDrawMode drawMode);
GLenum to_shader_type(EShadingStage shadingStage);
GLenum to_filter_type(EFilterMode filterMode);
GLenum to_wrap_type(EWrapMode wrapMode);

GLenum to_base_format(GLenum internalFormat);
uint32 num_pixel_components(GLenum internalFormat);
bool is_color_format(GLenum internalFormat);

}// end namespace ph::editor::ghi::opengl
