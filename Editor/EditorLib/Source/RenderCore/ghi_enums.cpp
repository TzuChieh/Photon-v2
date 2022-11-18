#include "RenderCore/ghi_enums.h"

#include <Common/assertion.h>

namespace ph::editor
{

std::size_t apparent_bytes_in_single_pixel(const EGHITextureFormat format)
{
	switch(format)
	{
	case EGHITextureFormat::RGB_8: return 3 * 1;
	case EGHITextureFormat::RGBA_8: return 4 * 1;
	case EGHITextureFormat::RGB_16F: return 3 * 2;
	case EGHITextureFormat::RGBA_16F: return 4 * 2;
	case EGHITextureFormat::RGB_32F: return 3 * 4;
	case EGHITextureFormat::RGBA_32F: return 4 * 4;
	case EGHITextureFormat::Depth_24_Stencil_8: return 3 + 1;
	default: PH_ASSERT_UNREACHABLE_SECTION(); return 0;
	}
}

}// end namespace ph::editor
