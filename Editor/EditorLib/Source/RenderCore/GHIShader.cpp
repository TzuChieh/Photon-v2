#include "RenderCore/GHIShader.h"

#include <utility>

namespace ph::editor
{

GHIShader::GHIShader(std::string name, const EGHIShadingStage shadingStage)
	: m_name(std::move(name))
	, m_shadingStage(shadingStage)
{}

GHIShader::~GHIShader() = default;

}// end namespace ph::editor
