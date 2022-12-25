#pragma once

#include "RenderCore/GHIShader.h"
#include "ThirdParty/glad2.h"

#include <string>

namespace ph::editor
{

class OpenglShader : public GHIShader
{
public:
	OpenglShader(
		std::string name, 
		EGHIInfoShadingStage shadingStage,
		std::string shaderSource);

	~OpenglShader() override;

	NativeHandle getNativeHandle() override;

private:
	GLuint m_shaderID;
};

}// end namespace ph::editor
