#pragma once

#include "ThirdParty/DearImGui.h"

#include <Common/assertion.h>

namespace ph::editor
{

class ImguiHelper final
{
public:
	ImFont* defaultFont = nullptr;
	ImFont* largeFont = nullptr;

public:
	template<typename UIFunc>
	void useFont(ImFont* font, UIFunc func);
};

template<typename UIFunc>
inline void ImguiHelper::useFont(ImFont* const font, UIFunc func)
{
	PH_ASSERT(font);

	ImGui::PushFont(font);
	func();
	ImGui::PopFont();
}

}// end namespace ph::editor
