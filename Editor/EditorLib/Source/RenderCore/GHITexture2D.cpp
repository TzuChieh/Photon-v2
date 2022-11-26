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

}// end namespace ph::editor
