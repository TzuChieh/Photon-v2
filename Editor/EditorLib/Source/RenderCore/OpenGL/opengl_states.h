#pragma once

#include "ThirdParty/glad2.h"

namespace ph::editor
{

class GHISampleState;

class OpenglSampleState final
{
public:
	GLenum filterType;
	GLenum wrapType;

	OpenglSampleState();
	explicit OpenglSampleState(const GHISampleState& ghiState);
};

inline OpenglSampleState::OpenglSampleState()
	: filterType(GL_LINEAR)
	, wrapType(GL_REPEAT)
{}

}// end namespace ph::editor
