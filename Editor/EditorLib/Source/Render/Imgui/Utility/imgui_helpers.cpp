#include "Render/Imgui/Utility/imgui_helpers.h"

#include <Common/assertion.h>

#include <algorithm>

namespace ph::editor::imgui
{

void copy_to(TSpan<char> dstBuffer, std::string_view srcStr)
{
	PH_ASSERT(!dstBuffer.empty());

	// Can only copy what the buffer can hold
	const auto numCharsToCopy = std::min(srcStr.size(), dstBuffer.size());
	std::copy(srcStr.begin(), srcStr.begin() + numCharsToCopy, dstBuffer.begin());

	// Always make the result null-terminated
	if(numCharsToCopy < dstBuffer.size())
	{
		dstBuffer[numCharsToCopy] = '\0';
	}
	else
	{
		dstBuffer.back() = '\0';
	}
}

}// end ph::editor::imgui
