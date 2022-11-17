#pragma once

#include "RenderCore/GHITexture2D.h"

namespace ph::editor
{

class OglTexture2D : public GHITexture2D
{
public:
	OglTexture2D(... math::TVector2<uint32> sizePx);
	virtual ~OglTexture2D();

	void* getNativeHandle() override;
};

}// end namespace ph::editor
