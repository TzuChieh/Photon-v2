#pragma once

#include "RenderCore/GHIShaderProgram.h"

#include "ThirdParty/glad2.h"

#include <Utility/utility.h>
#include <Utility/string_utils.h>

#include <memory>
#include <string>

namespace ph::editor
{

class GHIShader;

class OpenglShaderProgram : public GHIShaderProgram
{
public:
	OpenglShaderProgram(
		std::string name,
		const std::shared_ptr<GHIShader>& vertexShader,
		const std::shared_ptr<GHIShader>& fragmentShader);

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

	static GLuint getOpenglHandle(GHIShader& shader);
	void checkLinkStatus(const GHIShader& vertexShader, const GHIShader& fragmentShader) const;
	void validateProgram(const GHIShader& vertexShader, const GHIShader& fragmentShader) const;
	void collectProgramUniforms();
	const Uniform* getUniform(GLenum type, std::string_view name) const;
	void warnUniformNotFound(const Uniform* uniform, GLenum intendedType, std::string_view intendedName) const;
	std::string getInfoLog() const;

	GLuint m_programID;
	string_utils::TStdUnorderedStringMap<Uniform> m_nameToUniform;
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

}// end namespace ph::editor
