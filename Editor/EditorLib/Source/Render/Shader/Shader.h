#pragma once

#include "Render/SceneResource.h"

#include <memory>

namespace ph::editor { class GHIShaderProgram; }

namespace ph::editor::render
{

class Shader : public SceneResource
{
public:
	Shader();

	~Shader() override;

	virtual GHIShaderProgram* getGHIShader() const = 0;
	virtual std::shared_ptr<GHIShaderProgram> getGHIShaderResource() const = 0;

	void setupGHI(GHIThreadCaller& caller) override = 0;
	void cleanupGHI(GHIThreadCaller& caller) override = 0;
};

}// end namespace ph::editor::render
