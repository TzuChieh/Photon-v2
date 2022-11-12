#pragma once

#include <Math/TVector2.h>

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

	/*! @brief Frame buffer resolution of the main drawing area.
	@note 0 if not applicable.
	*/
	math::Vector2S frameBufferSizePx = {0, 0};
};

}// end namespace ph::editor
