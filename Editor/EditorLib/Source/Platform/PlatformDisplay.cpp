#include "Platform/PlatformDisplay.h"

namespace ph::editor
{

PlatformDisplay::PlatformDisplay() = default;

PlatformDisplay::~PlatformDisplay() = default;

PlatformDisplay::NativeWindow PlatformDisplay::getNativeWindow() const
{
	// No native window available
	return std::monostate();
}

}// end namespace ph::editor
