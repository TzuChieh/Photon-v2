#pragma once

#include "RenderCore/ShaderProgram.h"

#include "ThirdParty/glad2.h"

#include <Container/TStdUnorderedStringMap.h>
#include <Utility/utility.h>

#include <memory>
#include <string>

namespace ph::editor::ghi
{

class Shader;

class OpenglShaderProgram : public ShaderProgram
{
public:
	OpenglShaderProgram(
		std::string name,
		const std::shared_ptr<Shader>& vertexShader,
		const std::shared_ptr<Shader>& fragmentShader);

	~OpenglShaderProgram() override;

	void bind() override;

	void setInt32(std::string_view name, int32 value) override;
	void setFloat32(std::string_view name, float32 value) override;
	void setVector3F(std::string_view name, const math::Vector3F& value) override;
	void setVector4F(std::string_view name, const math::Vector4F& value) override;
	void setMatrix4F(std::string_view name, const math::Matrix4F& value) override;

private:
	struct Uniform final
	{
		GLint location = -1;
		GLsizei arraySize = 0;
		GLenum type = GL_NONE;
	};

	static GLuint getOpenglHandle(Shader& shader);
	void checkLinkStatus(const Shader& vertexShader, const Shader& fragmentShader) const;
	void validateProgram(const Shader& vertexShader, const Shader& fragmentShader) const;
	void collectProgramUniforms();
	const Uniform* getUniform(GLenum type, std::string_view name) const;
	void warnUniformNotFound(const Uniform* uniform, GLenum intendedType, std::string_view intendedName) const;
	std::string getInfoLog() const;

	GLuint m_programID;
	TStdUnorderedStringMap<Uniform> m_nameToUniform;
};

inline auto OpenglShaderProgram::getUniform(const GLenum type, std::string_view name) const
-> const Uniform*
{
	const auto& iter = m_nameToUniform.find(name);
	const Uniform* uniform = nullptr;
	if(iter != m_nameToUniform.cend() && iter->second.type == type)
	{
		uniform = &(iter->second);
	}
	else
	{
		uniform = nullptr;
		warnUniformNotFound(uniform, type, name);
	}

	return uniform;
}

}// end namespace ph::editor::ghi
