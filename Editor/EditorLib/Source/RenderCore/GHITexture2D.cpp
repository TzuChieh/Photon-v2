#include "RenderCore/GHITexture2D.h"

namespace ph::editor
{

GHITexture2D::~GHITexture2D() = default;

GHITexture2D::GHITexture2D()
	: GHITexture2D({0, 0}, EGHITextureFormat::RGB_8)
{}

GHITexture2D::GHITexture2D(const math::TVector2<uint32> sizePx, const EGHITextureFormat format)
	: m_sizePx(sizePx)
	, m_format(format)
{}

}// end namespace ph::editor
