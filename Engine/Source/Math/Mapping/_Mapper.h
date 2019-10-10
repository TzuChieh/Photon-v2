#pragma once

#include "Core/Bound/TAABB2D.h"
#include "Math/Mapping/TCoordinate2_i0e1.h"
#include "Math/TVector2.h"
#include "Common/assertion.h"

namespace ph::math
{

//class Mapper final
//{
//public:
//	template<typename T>
//	static TVector2<T> toRegion(const TCoordinate2_i0e1<T>& coord, const TAABB2D<T>& region);
//};
//
//// Implementations:
//
//template<typename T>
//inline TVector2<T> Mapper::toRegion(const TCoordinate2_i0e1<T>& coord, const TAABB2D<T>& region)
//{
//	PH_ASSERT(region.isValid());
//
//	return TVector2<T>(
//		coord.getCoordinates().x * region.getWidth()  + region.minVertex.x,
//		coord.getCoordinates().y * region.getHeight() + region.minVertex.y);
//}

}// end namespace ph::math
