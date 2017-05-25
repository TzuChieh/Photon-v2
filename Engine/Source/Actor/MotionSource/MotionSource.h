#pragma once

#include "Common/primitive_type.h"
#include "Math/Transform/TDecomposedTransform.h"

namespace ph
{

class MotionSource
{
public:
	virtual ~MotionSource() = 0;

	virtual TDecomposedTransform<hiReal> genNextTransform(const TDecomposedTransform<hiReal>& current) const = 0;
};

}// end namespace ph