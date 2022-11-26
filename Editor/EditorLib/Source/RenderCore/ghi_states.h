#pragma once

#include "RenderCore/ghi_enums.h"

namespace ph::editor
{

class GHIInfoSampleState final
{
public:
	EGHIInfoFilterMode filterMode;
	EGHIInfoWrapMode wrapMode;

	GHIInfoSampleState();
};

inline GHIInfoSampleState::GHIInfoSampleState()
	: filterMode(EGHIInfoFilterMode::Linear)
	, wrapMode(EGHIInfoWrapMode::Repeat)
{}

}// end namespace ph::editor
