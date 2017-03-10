#pragma once

#include "Common/primitive_type.h"
#include "Math/math_fwd.h"

namespace ph
{

class Frame
{
public:
	virtual ~Frame() = 0;

	virtual void resize(const uint32 newWidthPx, const uint32 newHeightPx) = 0;
	virtual void getPixel(const uint32 x, const uint32 y, Vector3R* const out_pixel) const = 0;
	virtual void setPixel(const uint32 x, const uint32 y, const real r, const real g, const real b) = 0;

	virtual uint32 getWidthPx() const = 0;
	virtual uint32 getHeightPx() const = 0;
	virtual uint32 numPixelComponents() const = 0;
	virtual const real* getPixelData() const = 0;
};

}// end namespace ph