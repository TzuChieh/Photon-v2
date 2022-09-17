#pragma once

#include "Platform/Display.h"

namespace ph::editor
{

class GlfwDisplay : public Display
{
public:
	void present() override;

	virtual math::Vector2S getSizePx() const = 0;
};

}// end namespace ph::editor
