#pragma once

#include "Math/TVector2.h"
#include "Core/Bound/TAABB2D.h"
#include "Common/assertion.h"

namespace ph
{

class UniformRectangle
{
public:
	template<typename T>
	static TVector2<T> map(const Vector2R& seed, const TAABB2D<T>& rectangle);
};

// In-header Implementations:

template<typename T>
inline TVector2<T> UniformRectangle::map(const Vector2R& seed, const TAABB2D<T>& rectangle)
{
	PH_ASSERT_LE(0.0_r, seed.x); PH_ASSERT_LE(seed.x, 1.0_r);
	PH_ASSERT_LE(0.0_r, seed.y); PH_ASSERT_LE(seed.y, 1.0_r);

	return rectangle.minVertex.add(seed.mul(rectangle.getExtents()));
}

}// end namespace ph