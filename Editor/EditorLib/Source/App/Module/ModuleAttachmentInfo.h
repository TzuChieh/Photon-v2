#pragma once

#include <Math/TVector2.h>

namespace ph::editor
{

class Editor;

class ModuleAttachmentInfo final
{
public:
	Editor*        editor            = nullptr;
	math::Vector2S frameBufferSizePx = {0, 0};
};

}// end namespace ph::editor
