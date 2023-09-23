#pragma once

#include "Render/ISceneResource.h"

#include <memory>

namespace ph::editor::ghi { class ShaderProgram; }

namespace ph::editor::render
{

class Shader : public ISceneResource
{
public:
	Shader();

	~Shader() override;

	virtual ghi::ShaderProgram* getGHIShader() const = 0;
	virtual std::shared_ptr<ghi::ShaderProgram> getGHIShaderResource() const = 0;

	void setupGHI(GHIThreadCaller& caller) override = 0;
	void cleanupGHI(GHIThreadCaller& caller) override = 0;
};

}// end namespace ph::editor::render
