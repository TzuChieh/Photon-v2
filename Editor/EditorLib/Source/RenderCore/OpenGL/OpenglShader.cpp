#include "RenderCore/OpenGL/OpenglShader.h"
#include "RenderCore/OpenGL/opengl_enums.h"

#include <Common/logging.h>

#include <utility>

namespace ph::editor
{

PH_DEFINE_INTERNAL_LOG_GROUP(OpenglShader, GHI);

OpenglShader::OpenglShader(
	std::string name, 
	const EGHIShadingStage shadingStage,
	std::string shaderSource)

	: GHIShader(std::move(name), shadingStage)

	, m_shaderID(0)
{
	m_shaderID = glCreateShader(opengl::translate(shadingStage));

	const GLchar* const shaderSourceCstr = shaderSource.c_str();
	glShaderSource(
		m_shaderID, 
		1,
		&shaderSourceCstr,
		nullptr);

	glCompileShader(m_shaderID);

	// Check compilation status and possibly log warning/error

	PH_LOG(OpenglShader,
		"[shader {}] compile log: {}", getName(), getInfoLog());

	GLint isCompiled;
	glGetShaderiv(m_shaderID, GL_COMPILE_STATUS, &isCompiled);
	if(isCompiled == GL_TRUE)
	{
		PH_LOG(OpenglShader, "[shader {}] compilation successed", getName());
	}
	else
	{
		PH_LOG_ERROR(OpenglShader, 
			"[shader {}] compilation failed, see log for detailed reason of failure", 
			getName());
	}
}

OpenglShader::~OpenglShader()
{
	glDeleteShader(m_shaderID);
}

auto OpenglShader::getNativeHandle()
-> NativeHandle
{
	if(m_shaderID != 0)
	{
		return static_cast<uint64>(m_shaderID);
	}
	else
	{
		return std::monostate{};
	}
}

std::string OpenglShader::getInfoLog() const
{
	GLint infoLogLength = 0;
	glGetShaderiv(m_shaderID, GL_INFO_LOG_LENGTH, &infoLogLength);

	// `infoLogLength` includes the NULL character \0
	std::string infoLog(infoLogLength, '\0');
	glGetShaderInfoLog(m_shaderID, infoLogLength, nullptr, infoLog.data());

	return infoLog;
}

}// end namespace ph::editor
