#include <Core/Intersectable/triangle_SIMD_Bvh/SIMD_triangle_intersection.h>
#include <Core/Ray.h>
#include <Math/TVector3.h>

#include <gtest/gtest.h>

TEST(RayWithPackedTriangleTest, HitReturnIsCorrect)
{
	using namespace ph;
	Ray r(Vector3R(1.0f,1.0f,1.0f), Vector3R(0.0f,0.0f,1.0f), 0.0f , 100.0f);
	// std::cout << r.getOrigin().x << "," << r.getOrigin().y << "," << r.getOrigin().z << std::endl;
	// std::cout << r.getDirection().x << "," << r.getDirection().y << "," << r.getDirection().z << std::endl;
	PackedTriangle tri;
	PackedIntersectionResult results;

	// this case should not appear( A triangle is a point ) ! 
	// tri.e1[0] = simdpp::splat(0.0f);
	// tri.e1[1] = simdpp::splat(0.0f);
	// tri.e1[2] = simdpp::splat(0.0f);

	// tri.e2[0] = simdpp::splat(0.0f);
	// tri.e2[1] = simdpp::splat(0.0f);
	// tri.e2[2] = simdpp::splat(0.0f);

	// tri.v0[0] = simdpp::splat(0.0f);
	// tri.v0[1] = simdpp::splat(0.0f);
	// tri.v0[2] = simdpp::splat(0.0f);

	// EXPECT_EQ(ray.isIntersectPackedTriangle(tri, results), false);

	//case : a ray perpendicular hit a triangle's vertex -> false

	Vector3R v0[8];
	Vector3R v1[8];
	Vector3R v2[8];
	

	for(int i = 0 ; i < 8 ; ++i)
	{
		Vector3R tp0(0, 0, i+2);
		Vector3R tp1(0, 3, i+2);
		Vector3R tp2(3,0, i+2);
		v0[i] = tp0;
		v1[i] = tp1;
		v2[i] = tp2;
	}
	// std::cout << "tris" <<std::endl;

	// for(int i =0 ;i < 8 ; i++)
	// {
	// 	std::cout << v0[i].x << "," << v0[i].y << "," << v0[i].z << std::endl;
	// 	std::cout << v1[i].x << "," << v1[i].y << "," << v1[i].z << std::endl;
	// 	std::cout << v2[i].x << "," << v2[i].y << "," << v2[i].z << std::endl;
	// }

	// std::cout << "end tris" << std::endl;
	//normal test
	tri.e1[0] = simdpp::make_float(v1[0].x-v0[0].x, v1[1].x-v0[1].x, v1[2].x-v0[2].x, v1[3].x-v0[3].x, v1[4].x-v0[4].x, v1[5].x-v0[5].x, v1[6].x-v0[6].x, v1[7].x-v0[7].x);
	tri.e1[1] = simdpp::make_float(v1[0].y-v0[0].y, v1[1].y-v0[1].y, v1[2].y-v0[2].y, v1[3].y-v0[3].y, v1[4].y-v0[4].y, v1[5].y-v0[5].y, v1[6].y-v0[6].y, v1[7].y-v0[7].y);
	tri.e1[2] = simdpp::make_float(v1[0].z-v0[0].z, v1[1].z-v0[1].z, v1[2].z-v0[2].z, v1[3].z-v0[3].z, v1[4].z-v0[4].z, v1[5].z-v0[5].z, v1[6].z-v0[6].z, v1[7].z-v0[7].z);
	
	tri.e2[0] = simdpp::make_float(v2[0].x-v0[0].x, v2[1].x-v0[1].x, v2[2].x-v0[2].x, v2[3].x-v0[3].x, v2[4].x-v0[4].x, v2[5].x-v0[5].x, v2[6].x-v0[6].x, v2[7].x-v0[7].x);
	tri.e2[1] = simdpp::make_float(v2[0].y-v0[0].y, v2[1].y-v0[1].y, v2[2].y-v0[2].y, v2[3].y-v0[3].y, v2[4].y-v0[4].y, v2[5].y-v0[5].y, v2[6].y-v0[6].y, v2[7].y-v0[7].y);
	tri.e2[2] = simdpp::make_float(v2[0].z-v0[0].z, v2[1].z-v0[1].z, v2[2].z-v0[2].z, v2[3].z-v0[3].z, v2[4].z-v0[4].z, v2[5].z-v0[5].z, v2[6].z-v0[6].z, v2[7].z-v0[7].z);

	tri.v0[0] = simdpp::make_float(v0[0].x, v0[1].x, v0[2].x, v0[3].x, v0[4].x, v0[5].x, v0[6].x, v0[7].x);
	tri.v0[1] = simdpp::make_float(v0[0].y, v0[1].y, v0[2].y, v0[3].y, v0[4].y, v0[5].y, v0[6].y, v0[7].y);
	tri.v0[2] = simdpp::make_float(v0[0].z, v0[1].z, v0[2].z, v0[3].z, v0[4].z, v0[5].z, v0[6].z, v0[7].z);

	simdpp::float32<width> all_zero = simdpp::splat(0.0f);
	tri.inactiveMask = simdpp::to_mask(all_zero);
	testRay ray(r);
	EXPECT_EQ(ray.isIntersectPackedTriangle(tri, results), true);
	EXPECT_EQ(results.idx, 0);
	EXPECT_EQ(results.t, 1);

	r.setOrigin(Vector3R(0.0f,0.0f,0.0f));
	EXPECT_EQ(ray.isIntersectPackedTriangle(tri, results), false);

	//case : a ray parrallel hit a triangle
	r.setOrigin(Vector3R(-1, 0, 2));
	r.setDirection(Vector3R(1, 0, 0));
	EXPECT_EQ(ray.isIntersectPackedTriangle(tri, results), false);

	unsigned seed = (unsigned)time(NULL);
	srand(seed);
	int X = 1000000;

	for(int i = 0; i < 1000000; i ++)
	{
		float r1 = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/X));
		float r2 = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/X));
		float r3 = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/X));

		float r4 = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/X));
		float r5 = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/X));
		float r6 = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/X));

		float r7 = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/X));
		float r8 = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/X));
		float r9 = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/X));

		normaltri[i].setVertex(Vector3R(r1, r2, r3), Vector3R(r4, r5, r6), Vector3R(r7 ,r8, r9));
		if( (i+1)%8 == 0)
		{
			std::vector<testTriangle> temp;
			for(int j = 0; j < 8; j ++)
			{
				temp.push_back(normaltri[i-j]);
			}
			packedtri[(i+1)/8].setVertex(temp);
		}


	}
	clock_t begin = clock();


	Vector3R outIntersectionPoint;
	for(int i = 0; i < 1000000; i++)
	{
		
		RayIntersectsTriangle(r.getOrigin(), 
                           	r.getDirection(), 
                           normaltri[i],
                           outIntersectionPoint);
	}
	clock_t end = clock();
	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
	std::cout << "normal triangle time" << elapsed_secs << std::endl;

	begin = clock();

	PackedIntersectionResult tp_results;
	for(int i = 0; i < 1000000/8; i++)
	{
		ray.isIntersectPackedTriangle(packedtri[i], tp_results);
	}

	end = clock();
	elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
	std::cout << "packed triangle time" << elapsed_secs << std::endl;

	
}
