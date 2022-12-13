#include "RenderCore/GHITexture.h"

namespace ph::editor
{

GHIInfoTextureFormat::GHIInfoTextureFormat()
	: pixelFormat(EGHIInfoPixelFormat::Empty)
	, sampleState()
{}

GHITexture::~GHITexture() = default;

GHITexture::GHITexture()
{}

}// end namespace ph::editor
