#pragma once

#include "Common/primitive_type.h"

namespace ph
{

class Frame
{
public:
	virtual ~Frame() = 0;

	virtual void resize(const uint32 newWidthPx, const uint32 newHeightPx) = 0;

	virtual uint32 getWidthPx() const = 0;
	virtual uint32 getHeightPx() const = 0;
};

}// end namespace ph