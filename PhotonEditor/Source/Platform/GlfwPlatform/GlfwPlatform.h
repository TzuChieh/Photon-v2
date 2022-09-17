#pragma once

#include "Platform/Platform.h"

#include <memory>

namespace ph::editor
{

class GlfwInput;
class GlfwDisplay;

class GlfwPlatform : public Platform
{
public:
	GlfwPlatform();

	void update(float64 deltaS) override;
	void render() override;

private:
	std::unique_ptr<GlfwInput>   m_input;
	std::unique_ptr<GlfwDisplay> m_display;
};

}// end namespace ph::editor
