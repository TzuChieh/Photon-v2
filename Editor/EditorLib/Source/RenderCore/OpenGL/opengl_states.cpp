#include "RenderCore/OpenGL/opengl_states.h"
#include "RenderCore/ghi_states.h"

#include <Common/assertion.h>

namespace ph::editor
{

OpenglSampleState::OpenglSampleState(const GHIInfoSampleState& ghiState)
	: OpenglSampleState()
{
	switch(ghiState.filterMode)
	{
	case EGHIInfoFilterMode::Point: filterType = GL_NEAREST; break;
	case EGHIInfoFilterMode::Linear: filterType = GL_LINEAR; break;
	default: PH_ASSERT_UNREACHABLE_SECTION(); break;
	}

	switch(ghiState.wrapMode)
	{
	case EGHIInfoWrapMode::ClampToEdge: wrapType = GL_CLAMP_TO_EDGE; break;
	case EGHIInfoWrapMode::Repeat: wrapType = GL_REPEAT; break;
	default: PH_ASSERT_UNREACHABLE_SECTION(); break;
	}
}

}// end namespace ph::editor
