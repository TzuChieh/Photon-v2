#include "Core/Intersectable/triangle_SIMD_Bvh/SIMD_triangle_intersection.h"
#include <Math/TVector3.h>
#include "Core/Ray.h"
#include <gtest/gtest.h>

TEST(RayWithPackedTriangleTest, HitReturnIsCorrect)
{
	Ray r(Vector3R(0,0,-1), Vector3R(1,1,1), 0 , 100);
	PackedTriangle tri;
	PackedIntersectionResult result;
	Vector3R v0[8];
	Vector3R v1[8];
	Vector3R v2[8];
	for(int i = 0 ; i < 8 ; ++i)
	{
		Vector3R tp0(i, i, 0);
		Vector3R tp1(i+2, i+2, 0);
		Vector3R tp2(i+2,i,0);
		v0[i] = tp0;
		v1[i] = tp1;
		v2[i] = tp2;
	}

	tri.e1[0] = simdpp::make_float(v1[0].x-v0[0].x, v1[1].x-v0[1].x, v1[2].x-v0[2].x, v1[3].x-v0[3].x, v1[4].x-v0[4].x, v1[5].x-v0[5].x, v1[6].x-v0[6].x, v1[7].x-v0[7].x);
	tri.e1[1] = simdpp::make_float(v1[0].y-v0[0].y, v1[1].y-v0[1].y, v1[2].y-v0[2].y, v1[3].y-v0[3].y, v1[4].y-v0[4].y, v1[5].y-v0[5].y, v1[6].y-v0[6].y, v1[7].y-v0[7].y);
	tri.e1[2] = simdpp::make_float(v1[0].z-v0[0].z, v1[1].z-v0[1].z, v1[2].z-v0[2].z, v1[3].z-v0[3].z, v1[4].z-v0[4].z, v1[5].z-v0[5].z, v1[6].z-v0[6].z, v1[7].z-v0[7].z);
	
	tri.e2[0] = simdpp::make_float(v2[0].x-v0[0].x, v2[1].x-v0[1].x, v2[2].x-v0[2].x, v2[3].x-v0[3].x, v2[4].x-v0[4].x, v2[5].x-v0[5].x, v2[6].x-v0[6].x, v2[7].x-v0[7].x);
	tri.e2[1] = simdpp::make_float(v2[0].y-v0[0].y, v2[1].y-v0[1].y, v2[2].y-v0[2].y, v2[3].y-v0[3].y, v2[4].y-v0[4].y, v2[5].y-v0[5].y, v2[6].y-v0[6].y, v2[7].y-v0[7].y);
	tri.e2[2] = simdpp::make_float(v2[0].z-v0[0].z, v2[1].z-v0[1].z, v2[2].z-v0[2].z, v2[3].z-v0[3].z, v2[4].z-v0[4].z, v2[5].z-v0[5].z, v2[6].z-v0[6].z, v2[7].z-v0[7].z);

	tri.v0[0] = simdpp::make_float(v0[0].x, v0[1].x, v0[2].x, v0[3].x, v0[4].x, v0[5].x, v0[6].x, v0[7].x);
	tri.v0[1] = simdpp::make_float(v0[0].y, v0[1].y, v0[2].y, v0[3].y, v0[4].y, v0[5].y, v0[6].y, v0[7].y);
	tri.v0[2] = simdpp::make_float(v0[0].z, v0[1].z, v0[2].z, v0[3].z, v0[4].z, v0[5].z, v0[6].z, v0[7].z);

	simdpp::float32 all_zero = simdpp::make_float(0,0,0,0,0,0,0,0);
	tri.inactiveMask = simdpp::to_mask(all_zero);
	testRay ray(r);
	ray.isIntersectPackedTriangle(tri, result);
	EXPECT_EQ(results.idx, 0);
}