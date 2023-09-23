#include "RenderCore/Shader.h"

#include <utility>

namespace ph::editor::ghi
{

Shader::Shader(std::string name, const EShadingStage shadingStage)
	: m_name(std::move(name))
	, m_shadingStage(shadingStage)
{}

Shader::~Shader() = default;

}// end namespace ph::editor::ghi
