#include "RenderCore/ShaderProgram.h"

#include <utility>

namespace ph::editor::ghi
{

ShaderProgram::ShaderProgram(std::string name)
	: m_name(std::move(name))
{}

ShaderProgram::~ShaderProgram() = default;

}// end namespace ph::editor::ghi
