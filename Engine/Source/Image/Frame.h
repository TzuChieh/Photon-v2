#pragma once

#include "Common/primitive_type.h"

namespace ph
{

class Vector3f;

class Frame
{
public:
	virtual ~Frame() = 0;

	virtual void resize(const uint32 newWidthPx, const uint32 newHeightPx) = 0;
	virtual void getPixel(const uint32 x, const uint32 y, Vector3f* const out_pixel) const = 0;
	virtual void setPixel(const uint32 x, const uint32 y, const float32 r, const float32 g, const float32 b) = 0;

	virtual uint32 getWidthPx() const = 0;
	virtual uint32 getHeightPx() const = 0;
};

}// end namespace ph