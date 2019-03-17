#include "SIMD_triangle_intersection.h"

namespace SIMDPP_ARCH_NAMESPACE {
 
void print_arch()
{
    std::cout << static_cast<unsigned>(simdpp::this_compile_arch()) << '\n';
}
 
}// end of  namespace SIMDPP_ARCH_NAMESPACE
SIMDPP_MAKE_DISPATCHER((void)(print_arch)());


namespace ph
{

testRay::testRay(const Ray& r)
{
    Vector3R o = r.getOrigin();
    Vector3R dir = r.getDirection();
    float length = std::abs(r.getMaxT() - r.getMinT()) * dir.length();
    m_origin[0] = simdpp::splat(o.x);
    m_origin[1] = simdpp::splat(o.y);
    m_origin[2] = simdpp::splat(o.z);
    m_direction[0] = simdpp::splat(dir.x);
    m_direction[1] = simdpp::splat(dir.y);
    m_direction[2] = simdpp::splat(dir.z);
    m_length = simdpp::splat(length);
}

bool testRay::isIntersectPackedTriangle(const PackedTriangle& packedTris, PackedIntersectionResult& result)
{
    //must sort the triangles first
    simdpp::float32<width> ray_cross_e2[3];
    avx_multi_cross(ray_cross_e2, m_direction, packedTris.e2);

    simdpp::float32<width> a = avx_multi_dot(packedTris.e1, ray_cross_e2);

    
    simdpp::float32<width> f = simdpp::div(oneM256 , a);

    simdpp::float32<width> s[3];
    avx_multi_sub(s, m_origin, packedTris.v0);

    simdpp::float32<width> u = simdpp::mul(f, avx_multi_dot(s, ray_cross_e2));

    simdpp::float32<width> q[3];
    avx_multi_cross(q, s, packedTris.e1);

    simdpp::float32<width> v = simdpp::mul(f, avx_multi_dot(m_direction, q));

    simdpp::float32<width> t = simdpp::mul(f, avx_multi_dot(packedTris.e2, q));

    // Failure conditions
    simdpp::mask_float32<width> failed = simdpp::bit_and(
        simdpp::cmp_gt(a, negativeEpsilonM256) ,
        simdpp::cmp_lt(a, positiveEpsilonM256)
    );
    

    failed = simdpp::bit_or(failed, simdpp::cmp_lt(u, zeroM256));
    //this one is trivial
    //failed = simdpp::bit_or(failed, simdpp::cmp_gt(u, oneM256));
    failed = simdpp::bit_or(failed, simdpp::cmp_lt(v, zeroM256));
    failed = simdpp::bit_or(failed, simdpp::cmp_gt(simdpp::add(u, v), oneM256));
    failed = simdpp::bit_or(failed, simdpp::cmp_lt(t, zeroM256));
    failed = simdpp::bit_or(failed, simdpp::cmp_gt(t, m_length));
    failed = simdpp::bit_or(failed, packedTris.inactiveMask);

    const simdpp::float32<width> tResults = simdpp::blend(minusOneM256, t, failed);

    //float temp[width];
    float* temp = (float*)&tResults;
    //simdpp::store(temp, tResults);

    int mask = 0;

    // fill mask using temp's signed bit
    // dst stands for mask, a stands for tResults
    //  int _mm256_movemask_ps (__m256 a)
    //  FOR j := 0 to 7
    // 	i := j*32
    // 	IF a[i+31]
    // 		dst[j] := 1
    // 	ELSE
    // 		dst[j] := 0
    // 	FI
    // ENDFOR
    // dst[MAX:8] := 0
    
    for (int i=0; i < width; ++i)
    {   
        if(std::signbit(temp[i]))
        {
            mask |= (1<<i);
        }
    }

    if (mask != 0xFF)
    {
        result.idx = -1;

        //float* ptr = (float*)&tResults;
        for (int i = 0; i < width; ++i)
        {
            
            //find the cloest hit point put the t of the ray in result.t
            if (temp[i] >= 0.0f && temp[i] < result.t)
            {
                result.t = temp[i];
                result.idx = i;
            }
        }

        return result.idx != -1;
    }

    return false;

}


}
