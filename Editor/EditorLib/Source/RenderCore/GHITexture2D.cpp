#include "RenderCore/GHITexture2D.h"

namespace ph::editor
{

GHITexture2D::~GHITexture2D() = default;

GHITexture2D::GHITexture2D(const GHIInfoTextureFormat& format)
	: GHITexture(format)
{}

}// end namespace ph::editor
