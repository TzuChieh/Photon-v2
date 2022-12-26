#include "RenderCore/OpenGL/OpenglShaderProgram.h"
#include "RenderCore/GHIShader.h"

#include <Common/primitive_type.h>
#include <Common/logging.h>
#include <Common/assertion.h>
#include <Utility/utility.h>

#include <utility>

namespace ph::editor
{

PH_DEFINE_INTERNAL_LOG_GROUP(OpenglShaderProgram, GHI);

OpenglShaderProgram::OpenglShaderProgram(
	std::string name,
	const std::shared_ptr<GHIShader>& vertexShader,
	const std::shared_ptr<GHIShader>& fragmentShader)

	: GHIShaderProgram(std::move(name))

	, m_programID(0)
	, m_nameToUniform()
{
	m_programID = glCreateProgram();

	PH_ASSERT(vertexShader);
	PH_ASSERT(fragmentShader);

	glAttachShader(m_programID, getOpenglHandle(*vertexShader));
	glAttachShader(m_programID, getOpenglHandle(*fragmentShader));

	glLinkProgram(m_programID);

	glDetachShader(m_programID, getOpenglHandle(*vertexShader));
	glDetachShader(m_programID, getOpenglHandle(*fragmentShader));

	checkLinkStatus(*vertexShader, *fragmentShader);
	validateProgram(*vertexShader, *fragmentShader);
	collectProgramUniforms();
}

OpenglShaderProgram::~OpenglShaderProgram()
{
	glDeleteProgram(m_programID);
}

void OpenglShaderProgram::bind()
{
	glUseProgram(m_programID);
}

void OpenglShaderProgram::setInt32(std::string_view name, const int32 value)
{

}

void OpenglShaderProgram::setFloat32(std::string_view name, const float32 value)
{

}

void OpenglShaderProgram::setVector3F(std::string_view name, const math::Vector3F& value)
{

}

void OpenglShaderProgram::setVector4F(std::string_view name, const math::Vector4F& value)
{

}

void OpenglShaderProgram::setMatrix4F(std::string_view name, const math::Matrix4F& value)
{

}

GLuint OpenglShaderProgram::getOpenglHandle(GHIShader& shader)
{
	const auto nativeHandle = shader.getNativeHandle();

	PH_ASSERT(std::holds_alternative<uint64>(nativeHandle));
	return lossless_cast<GLuint>(std::get<uint64>(nativeHandle));
}

void OpenglShaderProgram::checkLinkStatus(const GHIShader& vertexShader, const GHIShader& fragmentShader) const
{
	GLint isLinked = GL_FALSE;
	glGetProgramiv(m_programID, GL_LINK_STATUS, &isLinked);
	if(isLinked == GL_FALSE)
	{
		PH_LOG_ERROR(OpenglShaderProgram,
			"{} link failed (against vs: {}, fs: {}): {}", 
			getName(), vertexShader.getName(), fragmentShader.getName(), getInfoLog());
	}
}

void OpenglShaderProgram::validateProgram(const GHIShader& vertexShader, const GHIShader& fragmentShader) const
{
	glValidateProgram(m_programID);

	GLint isValidated = GL_FALSE;
	glGetProgramiv(m_programID, GL_VALIDATE_STATUS, &isValidated);
	if(isValidated == GL_FALSE)
	{
		PH_LOG_ERROR(OpenglShaderProgram,
			"{} validation failed (against vs: {}, fs: {}): {}",
			getName(), vertexShader.getName(), fragmentShader.getName(), getInfoLog());
	}
}

void OpenglShaderProgram::collectProgramUniforms()
{
	GLint numUniforms = 0;
	glGetProgramiv(m_programID, GL_ACTIVE_UNIFORMS, &numUniforms);

	if(numUniforms != 0)
	{
		GLint maxUniformNameLength = 0;
		glGetProgramiv(m_programID, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxUniformNameLength);

		auto uniformNameBuffer = std::make_unique<char[]>(maxUniformNameLength);
		for(GLint ui = 0; ui < numUniforms; ++ui)
		{
			GLsizei uniformNameLength = 0;
			GLsizei uniformArraySize = 0;
			GLenum uniformType = GL_NONE;
			glGetActiveUniform(
				m_programID, 
				ui, 
				maxUniformNameLength, 
				&uniformNameLength, 
				&uniformArraySize, 
				&uniformType, 
				uniformNameBuffer.get());

			Uniform uniform;
			uniform.location = glGetUniformLocation(m_programID, uniformNameBuffer.get());
			uniform.arraySize = uniformArraySize;
			uniform.type = uniformType;

			m_nameToUniform[std::string(uniformNameBuffer.get(), uniformNameLength)] = uniform;
		}
	}
}

std::string OpenglShaderProgram::getInfoLog() const
{
	// The information log for a program object is modified when the program object 
	// is linked or validated.

	GLint infoLogLength = 0;
	glGetProgramiv(m_programID, GL_INFO_LOG_LENGTH, &infoLogLength);

	// `infoLogLength` includes the NULL character \0
	std::string infoLog(infoLogLength, '\0');
	glGetProgramInfoLog(m_programID, infoLogLength, nullptr, infoLog.data());

	return infoLog;
}

}// end namespace ph::editor
