#pragma once

#include "ThirdParty/DearImGui.h"

namespace ph::editor
{

class ImguiEditorTheme final
{
public:
	ImVec4 warningColor = ImVec4(1.0f, 1.0f, 0.2f, 1.0f);
	ImVec4 errorColor = ImVec4(1.0f, 0.2f, 0.2f, 1.0f);
};

}// end namespace ph::editor
