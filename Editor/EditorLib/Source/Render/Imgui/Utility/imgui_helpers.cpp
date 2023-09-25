#include "Render/Imgui/Utility/imgui_helpers.h"
#include "Render/Imgui/Font/imgui_icons.h"

#include <Common/assertion.h>

#include <algorithm>

namespace ph::editor::imgui
{

void text_unformatted(std::string_view text)
{
	ImGui::TextUnformatted(text.data(), text.data() + text.size());
}

void image_with_fallback(
	ImTextureID textureID,
	const math::Vector2F& sizePx,
	const math::Vector4F& tintColorRGBA,
	const math::Vector4F& borderColorRGBA)
{
	if(!textureID)
	{
		// Indicate the image is unavailable for now
		ImGui::TextUnformatted(PH_IMGUI_LOADING_ICON " Loading...");
		return;
	}

	ImGui::Image(
		textureID,
		ImVec2(sizePx.x(), sizePx.y()),
		ImVec2(0, 1),// `uv0` is at upper-left corner
		ImVec2(1, 0),// `uv1` is at lower-right corner
		ImVec4(tintColorRGBA.r(), tintColorRGBA.g(), tintColorRGBA.b(), tintColorRGBA.a()),
		ImVec4(borderColorRGBA.r(), borderColorRGBA.g(), borderColorRGBA.b(), borderColorRGBA.a()));
}

bool image_button_with_fallback(
	const char* strId,
	ImTextureID textureID,
	const math::Vector2F& sizePx,
	const math::Vector4F& backgroundColorRGBA,
	const math::Vector4F& tintColorRGBA)
{
	if(!textureID)
	{
		// Indicate the image is unavailable for now
		// FIXME: draw square button with text as large as image button size
		ImGui::TextUnformatted(PH_IMGUI_LOADING_ICON " Loading...");
	}

	return ImGui::ImageButton(
		strId,
		textureID,
		ImVec2(sizePx.x(), sizePx.y()),
		ImVec2(0, 1),// `uv0` is at upper-left corner
		ImVec2(1, 0),// `uv1` is at lower-right corner
		ImVec4(backgroundColorRGBA.r(), backgroundColorRGBA.g(), backgroundColorRGBA.b(), backgroundColorRGBA.a()),
		ImVec4(tintColorRGBA.r(), tintColorRGBA.g(), tintColorRGBA.b(), tintColorRGBA.a()));
}

void copy_to(TSpan<char> dstBuffer, std::string_view srcStr)
{
	PH_ASSERT(!dstBuffer.empty());

	// Can only copy what the buffer can hold
	const auto numCharsToCopy = std::min(srcStr.size(), dstBuffer.size());
	std::copy(srcStr.begin(), srcStr.begin() + numCharsToCopy, dstBuffer.begin());

	// Always make the result null-terminated
	if(numCharsToCopy < dstBuffer.size())
	{
		dstBuffer[numCharsToCopy] = '\0';
	}
	else
	{
		dstBuffer.back() = '\0';
	}
}

}// end ph::editor::imgui
