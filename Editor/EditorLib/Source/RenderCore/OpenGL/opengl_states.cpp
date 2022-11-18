#include "RenderCore/OpenGL/opengl_states.h"
#include "RenderCore/ghi_states.h"

#include <Common/assertion.h>

namespace ph::editor
{

OpenglSampleState::OpenglSampleState(const GHISampleState& ghiState)
	: OpenglSampleState()
{
	switch(ghiState.filterMode)
	{
	case EGHIFilterMode::Point: filterType = GL_NEAREST; break;
	case EGHIFilterMode::Linear: filterType = GL_LINEAR; break;
	default: PH_ASSERT_UNREACHABLE_SECTION(); break;
	}

	switch(ghiState.wrapMode)
	{
	case EGHIWrapMode::ClampToEdge: wrapType = GL_CLAMP_TO_EDGE; break;
	case EGHIWrapMode::Repeat: wrapType = GL_REPEAT; break;
	default: PH_ASSERT_UNREACHABLE_SECTION(); break;
	}
}

}// end namespace ph::editor
