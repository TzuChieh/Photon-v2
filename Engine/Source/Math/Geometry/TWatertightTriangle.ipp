#pragma once

#include "Math/Geometry/TWatertightTriangle.h"
#include "Common/assertion.h"
#include "Common/primitive_type.h"
#include "Math/TVector3.h"

#include <string>
#include <cmath>

namespace ph::math
{

template<typename T>
inline bool TWatertightTriangle<T>::isIntersecting(
	const TLineSegment<T>& segment,
	T* const               out_hitT,
	TVector3<T>* const     out_hitBarycentricCoords) const
{
	PH_ASSERT(out_hitT);
	PH_ASSERT(out_hitBarycentricCoords);

	TVector3<T> segmentDir = segment.getDirection();
	TVector3<T> vAt        = this->getVa().sub(segment.getOrigin());
	TVector3<T> vBt        = this->getVb().sub(segment.getOrigin());
	TVector3<T> vCt        = this->getVc().sub(segment.getOrigin());

	// Find the dominant dimension of ray direction and make it Z; the rest 
	// dimensions are arbitrarily assigned
	if(std::abs(segmentDir.x) > std::abs(segmentDir.y))
	{
		// X dominant
		if(std::abs(segmentDir.x) > std::abs(segmentDir.z))
		{
			segmentDir.set(segmentDir.y, segmentDir.z, segmentDir.x);
			vAt.set(vAt.y, vAt.z, vAt.x);
			vBt.set(vBt.y, vBt.z, vBt.x);
			vCt.set(vCt.y, vCt.z, vCt.x);
		}
		// Z dominant
		else
		{
			// left as-is
		}
	}
	else
	{
		// Y dominant
		if(std::abs(segmentDir.y) > std::abs(segmentDir.z))
		{
			segmentDir.set(segmentDir.z, segmentDir.x, segmentDir.y);
			vAt.set(vAt.z, vAt.x, vAt.y);
			vBt.set(vBt.z, vBt.x, vBt.y);
			vCt.set(vCt.z, vCt.x, vCt.y);
		}
		// Z dominant
		else
		{
			// left as-is
		}
	}

	PH_ASSERT_MSG(segmentDir.z != T(0) && std::isfinite(segmentDir.z), std::to_string(segmentDir.z));

	const T rcpSegmentDirZ = T(1) / segmentDir.z;
	const T shearX         = -segmentDir.x * rcpSegmentDirZ;
	const T shearY         = -segmentDir.y * rcpSegmentDirZ;
	const T shearZ         = rcpSegmentDirZ;

	vAt.x += shearX * vAt.z;
	vAt.y += shearY * vAt.z;
	vBt.x += shearX * vBt.z;
	vBt.y += shearY * vBt.z;
	vCt.x += shearX * vCt.z;
	vCt.y += shearY * vCt.z;

	T funcEa = vBt.x * vCt.y - vBt.y * vCt.x;
	T funcEb = vCt.x * vAt.y - vCt.y * vAt.x;
	T funcEc = vAt.x * vBt.y - vAt.y * vBt.x;

	// FIXME: properly check if T is of lower precision than float64
	// Possibly fallback to higher precision test for triangle edges
	if constexpr(sizeof(T) < sizeof(float64))
	{
		if(funcEa == T(0) || funcEb == T(0) || funcEc == T(0))
		{
			const float64 funcEa64 = static_cast<float64>(vBt.x) * static_cast<float64>(vCt.y) -
			                         static_cast<float64>(vBt.y) * static_cast<float64>(vCt.x);
			const float64 funcEb64 = static_cast<float64>(vCt.x) * static_cast<float64>(vAt.y) -
			                         static_cast<float64>(vCt.y) * static_cast<float64>(vAt.x);
			const float64 funcEc64 = static_cast<float64>(vAt.x) * static_cast<float64>(vBt.y) -
			                         static_cast<float64>(vAt.y) * static_cast<float64>(vBt.x);
			
			funcEa = static_cast<T>(funcEa64);
			funcEb = static_cast<T>(funcEb64);
			funcEc = static_cast<T>(funcEc64);
		}
	}

	if((funcEa < T(0) || funcEb < T(0) || funcEc < T(0)) && 
	   (funcEa > T(0) || funcEb > T(0) || funcEc > T(0)))
	{
		return false;
	}

	const T determinant = funcEa + funcEb + funcEc;

	if(determinant == T(0))
	{
		return false;
	}

	vAt.z *= shearZ;
	vBt.z *= shearZ;
	vCt.z *= shearZ;

	const T hitTscaled = funcEa * vAt.z + funcEb * vBt.z + funcEc * vCt.z;

	if(determinant > T(0))
	{
		if(hitTscaled < segment.getMinT() * determinant || hitTscaled > segment.getMaxT() * determinant)
		{
			return false;
		}
	}
	else
	{
		if(hitTscaled > segment.getMinT() * determinant || hitTscaled < segment.getMaxT() * determinant)
		{
			return false;
		}
	}

	// So the ray intersects the triangle

	PH_ASSERT_MSG(determinant != T(0) && std::isfinite(determinant), std::to_string(determinant));

	const T rcpDeterminant = T(1) / determinant;
	const T baryA          = funcEa * rcpDeterminant;
	const T baryB          = funcEb * rcpDeterminant;
	const T baryC          = funcEc * rcpDeterminant;
	const T hitT           = hitTscaled * rcpDeterminant;

	*out_hitT                 = hitT;
	*out_hitBarycentricCoords = TVector3<T>(baryA, baryB, baryC);
	return true;
}

}// end namespace ph::math
