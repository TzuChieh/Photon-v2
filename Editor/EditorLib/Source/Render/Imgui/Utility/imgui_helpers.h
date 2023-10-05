#pragma once

#include "ThirdParty/DearImGui.h"

#include <Common/assertion.h>
#include <Utility/TSpan.h>
#include <Math/TVector2.h>
#include <Math/TVector4.h>

#include <cstddef>
#include <string_view>
#include <vector>

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

class StringCache final
{
public:
	StringCache();
	explicit StringCache(std::size_t cacheSize);

	/*! @brief Copy `inputContent` into cache without changing the cache's size.
	The resulting content may get truncated if there is not enough space in the cache.
	*/
	void fixedCopy(TSpanView<char> inputContent);

	/*! @brief Fully copy `inputContent` into cache. Resize the cache if needed.
	*/
	void resizableCopy(TSpanView<char> inputContent);

	bool inputText(
		const char* idName,
		ImGuiInputTextFlags flags = 0);

	bool inputText(
		const char* idName,
		TSpanView<char> inputContent,
		ImGuiInputTextFlags flags = 0);
	
	/*! @brief Make the cache empty.
	*/
	void clear();

	/*! @brief Get cached data.
	The resulting span is over the full cache and multiple null terminators may present.
	*/
	///@{
	TSpan<char> getData();
	TSpanView<char> getData() const;
	///@}

	/*! @brief Get the string in cache. The result should always be null terminated.
	*/
	///@{
	char* getContent();
	const char* getContent() const;
	///@}

	/*!
	@return `false` if there is any non-null characters in cache; otherwise, `true` is returned.
	Note that for empty string (contains only a null terminator) is considered empty.
	*/
	bool isEmpty() const;



private:
	std::vector<char> m_cache;
};

inline void StringCache::clear()
{
	PH_ASSERT(!m_cache.empty());
	m_cache.front() = '\0';
}

inline TSpan<char> StringCache::getData()
{
	return m_cache;
}

inline TSpanView<char> StringCache::getData() const
{
	return m_cache;
}

inline char* StringCache::getContent()
{
	return m_cache.data();
}

inline const char* StringCache::getContent() const
{
	return m_cache.data();
}

inline bool StringCache::isEmpty() const
{
	return !m_cache.empty() && m_cache.front() != '\0';
}

}// end ph::editor::imgui
