#pragma once

#include "ThirdParty/glad2.h"
#include "RenderCore/ghi_enums.h"

namespace ph::editor
{

GLenum to_OpenGL_internal_format(EGHITextureFormat format);
EGHITextureFormat from_OpenGL_internal_format(GLenum internalFormat);

}// end namespace ph::editor
