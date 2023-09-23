#pragma once

#include "RenderCore/Shader.h"

#include "ThirdParty/glad2.h"

#include <string>

namespace ph::editor::ghi
{

class OpenglShader : public Shader
{
public:
	OpenglShader(
		std::string name, 
		EShadingStage shadingStage,
		std::string shaderSource);

	~OpenglShader() override;

	NativeHandle getNativeHandle() override;

private:
	std::string getInfoLog() const;

	GLuint m_shaderID;
};

}// end namespace ph::editor::ghi
