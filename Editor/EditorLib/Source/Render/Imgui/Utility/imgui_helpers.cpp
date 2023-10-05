#include "Render/Imgui/Utility/imgui_helpers.h"
#include "Render/Imgui/Font/imgui_icons.h"

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

StringCache::StringCache()
	: StringCache(1)// single null terminator
{}

StringCache::StringCache(std::size_t cacheSize)
	: m_cache(cacheSize > 0 ? cacheSize : 1, '\0')
{}

void StringCache::fixedCopy(TSpanView<char> inputContent)
{
	copy_to(m_cache, {inputContent.begin(), inputContent.end()});
}

void StringCache::resizableCopy(TSpanView<char> inputContent)
{
	// Resize if cannot fit (+1 for null terminator)
	if(inputContent.size() + 1 > m_cache.size())
	{
		m_cache.resize(inputContent.size() + 1);
	}

	fixedCopy(inputContent);
}

bool StringCache::inputText(
	const char* idName,
	ImGuiInputTextFlags flags)
{
	auto resizeCallback = [](ImGuiInputTextCallbackData* cbData) -> int
	{
		if(cbData->EventFlag == ImGuiInputTextFlags_CallbackResize)
		{
			auto stdVec = reinterpret_cast<std::vector<char>*>(cbData->UserData);
			PH_ASSERT(stdVec->data() == cbData->Buf);

			// On resizing calls, generally `cbData->BufSize == cbData->BufTextLen + 1`
			stdVec->resize(cbData->BufSize);

			cbData->Buf = stdVec->data();
		}
		return 0;
	};

	return ImGui::InputText(
		idName,
		m_cache.data(),
		m_cache.size(),
		flags | ImGuiInputTextFlags_CallbackResize,
		resizeCallback,
		&m_cache);

	// A sidenote: Input text seems to cache buffer size internally (cannot alter input text buffer
	// size without closing then repoening the dialog window). Specifying resize callback works fine.
}

bool StringCache::inputText(
	const char* idName,
	TSpanView<char> inputContent,
	ImGuiInputTextFlags flags)
{
	// Copy input content into cache to display on UI
	resizableCopy(inputContent);

	return inputText(idName, flags);
}

}// end ph::editor::imgui
