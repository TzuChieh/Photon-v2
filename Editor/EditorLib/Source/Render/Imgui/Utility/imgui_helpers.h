#pragma once

#include "ThirdParty/DearImGui.h"

#include <Utility/TSpan.h>
#include <Math/TVector2.h>
#include <Math/TVector4.h>

#include <string_view>

namespace ph::editor::imgui
{

void text_unformatted(std::string_view text);

void image_with_fallback(
	ImTextureID textureID,
	const math::Vector2F& sizePx,
	const math::Vector4F& tintColorRGBA = math::Vector4F(1, 1, 1, 1),
	const math::Vector4F& borderColorRGBA = math::Vector4F(0, 0, 0, 0));

bool image_button_with_fallback(
	const char* strId,
	ImTextureID textureID,
	const math::Vector2F& sizePx,
	const math::Vector4F& backgroundColorRGBA = math::Vector4F(0, 0, 0, 0),
	const math::Vector4F& tintColorRGBA = math::Vector4F(1, 1, 1, 1));

/*! @brief Copy string to a buffer. The result is always null-terminated.
@param dstBuffer The buffer to copy into. Its size should never be 0. 
*/
void copy_to(TSpan<char> dstBuffer, std::string_view srcStr);

}// end ph::editor::imgui
