#include "RenderCore/GHITexture2D.h"

namespace ph::editor
{

GHIInfoTextureFormat::GHIInfoTextureFormat()
	: pixelFormat(EGHIInfoPixelFormat::Empty)
	, sampleState()
{}

GHITexture2D::~GHITexture2D() = default;

GHITexture2D::GHITexture2D()
{}

auto GHITexture2D::getNativeHandle()
-> NativeHandle
{
	return std::monostate();
}

}// end namespace ph::editor
