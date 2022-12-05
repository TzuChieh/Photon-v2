#pragma once

#include "ThirdParty/DearImGui.h"

#include <Common/assertion.h>

namespace ph::editor
{

/*!
Dear ImGui can handle font loading, hence the font data (`ImFont`) here are never null after
initialization. Some utilities are provided for using the font data.
*/
class ImguiFontLibrary final
{
public:
	ImFont* defaultFont = nullptr;
	ImFont* largeFont = nullptr;

public:
	template<typename UIFunc>
	void useFont(ImFont* font, UIFunc func);
};

template<typename UIFunc>
inline void ImguiFontLibrary::useFont(ImFont* const font, UIFunc func)
{
	PH_ASSERT(font);

	ImGui::PushFont(font);
	func();
	ImGui::PopFont();
}

}// end namespace ph::editor
