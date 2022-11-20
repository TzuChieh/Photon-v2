#pragma once

#include <Math/TVector2.h>

#include <vector>
#include <string>

namespace ph::editor
{

class Platform;
class Editor;

class ModuleAttachmentInfo final
{
public:
	/*!
	@note Never empty.
	*/
	Platform* platform = nullptr;

	/*!
	@note Never empty.
	*/
	Editor* editor = nullptr;

	/*! @brief Framebuffer resolution of the main drawing area.
	@note 0 if not applicable.
	*/
	math::Vector2S framebufferSizePx = {0, 0};

	std::vector<std::string> attachedModuleNames;
};

}// end namespace ph::editor
