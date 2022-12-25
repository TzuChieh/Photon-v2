#pragma once

#include "RenderCore/GHIShaderProgram.h"

#include <memory>

namespace ph::editor
{

class GHIShader;

class OpenglShaderProgram : public GHIShaderProgram
{
public:
	OpenglShaderProgram(
		const std::shared_ptr<GHIShader>& vertexShader,
		const std::shared_ptr<GHIShader>& fragmentShader);

	~OpenglShaderProgram() override;

	void bind() override;
};

}// end namespace ph::editor
