#include "RenderCore/GHITexture2D.h"

namespace ph::editor
{

GHITexture2D::~GHITexture2D() = default;

GHITexture2D::GHITexture2D()
	: GHITexture2D({0, 0})
{}

GHITexture2D::GHITexture2D(const math::TVector2<uint32> sizePx)
	: m_sizePx(sizePx)
{}

}// end namespace ph::editor
