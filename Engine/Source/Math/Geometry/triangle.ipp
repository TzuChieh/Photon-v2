#pragma once

#include "Common/assertion.h"
#include "Math/Geometry/triangle.h"
#include "Math/TVector3.h"

namespace ph
{

namespace math
{

inline bool is_intersecting_watertight_triangle(
	const Ray&      ray,
	const Vector3R& vA,
	const Vector3R& vB,
	const Vector3R& vC,
	real* const     out_hitT,
	Vector3R* const out_hitBaryABCs)
{
	PH_ASSERT(out_hitT);
	PH_ASSERT(out_hitBaryABCs);

	Vector3R rayDir = ray.getDirection();
	Vector3R vAt = vA.sub(ray.getOrigin());
	Vector3R vBt = vB.sub(ray.getOrigin());
	Vector3R vCt = vC.sub(ray.getOrigin());

	// Find the dominant dimension of ray direction and make it Z; the rest 
	// dimensions are arbitrarily assigned
	if(std::abs(rayDir.x) > std::abs(rayDir.y))
	{
		// X dominant
		if(std::abs(rayDir.x) > std::abs(rayDir.z))
		{
			rayDir.set(rayDir.y, rayDir.z, rayDir.x);
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
		if(std::abs(rayDir.y) > std::abs(rayDir.z))
		{
			rayDir.set(rayDir.z, rayDir.x, rayDir.y);
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

	PH_ASSERT_MSG(rayDir.z != 0.0_r && std::isfinite(rayDir.z), std::to_string(rayDir.z));

	const real reciRayDirZ = 1.0_r / rayDir.z;
	const real shearX = -rayDir.x * reciRayDirZ;
	const real shearY = -rayDir.y * reciRayDirZ;
	const real shearZ = reciRayDirZ;

	vAt.x += shearX * vAt.z;
	vAt.y += shearY * vAt.z;
	vBt.x += shearX * vBt.z;
	vBt.y += shearY * vBt.z;
	vCt.x += shearX * vCt.z;
	vCt.y += shearY * vCt.z;

	real funcEa = vBt.x * vCt.y - vBt.y * vCt.x;
	real funcEb = vCt.x * vAt.y - vCt.y * vAt.x;
	real funcEc = vAt.x * vBt.y - vAt.y * vBt.x;

	// Possibly fallback to higher precision test for triangle edges
	if constexpr(sizeof(real) < sizeof(float64))
	{
		if(funcEa == 0.0_r || funcEb == 0.0_r || funcEc == 0.0_r)
		{
			const float64 funcEa64 = static_cast<float64>(vBt.x) * static_cast<float64>(vCt.y) -
			                         static_cast<float64>(vBt.y) * static_cast<float64>(vCt.x);
			const float64 funcEb64 = static_cast<float64>(vCt.x) * static_cast<float64>(vAt.y) -
			                         static_cast<float64>(vCt.y) * static_cast<float64>(vAt.x);
			const float64 funcEc64 = static_cast<float64>(vAt.x) * static_cast<float64>(vBt.y) -
			                         static_cast<float64>(vAt.y) * static_cast<float64>(vBt.x);
			
			funcEa = static_cast<real>(funcEa64);
			funcEb = static_cast<real>(funcEb64);
			funcEc = static_cast<real>(funcEc64);
		}
	}

	if((funcEa < 0.0_r || funcEb < 0.0_r || funcEc < 0.0_r) && (funcEa > 0.0_r || funcEb > 0.0_r || funcEc > 0.0_r))
	{
		return false;
	}

	const real determinant = funcEa + funcEb + funcEc;

	if(determinant == 0.0_r)
	{
		return false;
	}

	vAt.z *= shearZ;
	vBt.z *= shearZ;
	vCt.z *= shearZ;

	const real hitTscaled = funcEa * vAt.z + funcEb * vBt.z + funcEc * vCt.z;

	if(determinant > 0.0_r)
	{
		if(hitTscaled < ray.getMinT() * determinant || hitTscaled > ray.getMaxT() * determinant)
		{
			return false;
		}
	}
	else
	{
		if(hitTscaled > ray.getMinT() * determinant || hitTscaled < ray.getMaxT() * determinant)
		{
			return false;
		}
	}

	// So the ray intersects the triangle

	PH_ASSERT_MSG(determinant != 0 && std::isfinite(determinant), std::to_string(determinant));

	const real reciDeterminant = 1.0_r / determinant;
	const real baryA = funcEa * reciDeterminant;
	const real baryB = funcEb * reciDeterminant;
	const real baryC = funcEc * reciDeterminant;
	const real hitT = hitTscaled * reciDeterminant;

	*out_hitT = hitT;
	*out_hitBaryABCs = Vector3R(baryA, baryB, baryC);
	return true;
}

}// end namespace math

}// end namespace ph
