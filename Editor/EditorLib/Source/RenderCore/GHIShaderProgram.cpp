#include "RenderCore/GHIShaderProgram.h"

#include <utility>

namespace ph::editor
{

GHIShaderProgram::GHIShaderProgram(std::string name)
	: m_name(std::move(name))
{}

GHIShaderProgram::~GHIShaderProgram() = default;

}// end namespace ph::editor
