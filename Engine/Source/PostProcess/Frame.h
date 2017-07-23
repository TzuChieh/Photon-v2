#pragma once

#include "Common/primitive_type.h"
#include "Math/math_fwd.h"

namespace ph
{

class Frame
{
public:
	virtual ~Frame() = 0;

	virtual void resize(uint32 newWidthPx, uint32 newHeightPx) = 0;
	virtual void getPixel(uint32 x, uint32 y, Vector3R* out_pixel) const = 0;
	virtual void setPixel(uint32 x, uint32 y, real r, real g, real b) = 0;

	virtual uint32 getWidthPx() const = 0;
	virtual uint32 getHeightPx() const = 0;
	virtual uint32 numPixelComponents() const = 0;
	virtual const real* getPixelData() const = 0;
};

}// end namespace ph