#pragma once

#include "RenderCore/ghi_fwd.h"

#include "ThirdParty/glad2.h"

namespace ph::editor
{

class OpenglSampleState final
{
public:
	GLenum filterType;
	GLenum wrapType;

	OpenglSampleState();
	explicit OpenglSampleState(const GHIInfoSampleState& ghiState);

	inline bool operator == (const OpenglSampleState& rhs) const = default;
};

inline OpenglSampleState::OpenglSampleState()
	: filterType(GL_LINEAR)
	, wrapType(GL_REPEAT)
{}

}// end namespace ph::editor
