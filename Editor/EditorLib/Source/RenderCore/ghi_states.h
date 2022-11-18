#pragma once

#include "RenderCore/ghi_enums.h"

namespace ph::editor
{

class GHISampleState final
{
public:
	EGHIFilterMode filterMode;
	EGHIWrapMode wrapMode;

	GHISampleState();
};

inline GHISampleState::GHISampleState()
	: filterMode(EGHIFilterMode::Linear)
	, wrapMode(EGHIWrapMode::Repeat)
{}

}// end namespace ph::editor
