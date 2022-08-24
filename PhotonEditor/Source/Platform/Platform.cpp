#include "Platform/Platform.h"

#include <utility>

namespace ph::editor
{

Platform::Platform(
	std::unique_ptr<Input>   input,
	std::unique_ptr<Display> display)

	: m_input  (std::move(input))
	, m_display(std::move(display))
{}


}// end namespace ph::editor
