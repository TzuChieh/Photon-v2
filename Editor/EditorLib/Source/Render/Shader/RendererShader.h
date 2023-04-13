#pragma once

#include "Render/RendererResource.h"

#include <memory>

namespace ph::editor
{

class GHIShaderProgram;

class RendererShader : public RendererResource
{
public:
	RendererShader();

	~RendererShader() override;

	virtual GHIShaderProgram* getGHIShader() const = 0;
	virtual std::shared_ptr<GHIShaderProgram> getGHIShaderResource() const = 0;

	void setupGHI(GHIThreadCaller& caller) override = 0;
	void cleanupGHI(GHIThreadCaller& caller) override = 0;
};

}// end namespace ph::editor
