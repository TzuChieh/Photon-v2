#include "SIMD_triangle_intersection.h"


namespace ph
{

testRay::testRay(const Ray& r)
{
    Vector3R o = r.getOrigin();
    Vector3R dir = r.getDirection();
    float length = std::abs(r.getMaxT() - r.getMinT()) * dir.length();
    m_origin[0] = simdpp::make_float(0, 0, 0, 0, 0, 0, 0, o.x);
    m_origin[1] = simdpp::make_float(0, 0, 0, 0, 0, 0, 0, o.y);
    m_origin[2] = simdpp::make_float(0, 0, 0, 0, 0, 0, 0, o.z);
    m_direction[0] = simdpp::make_float(0, 0, 0, 0, 0, 0, 0, dir.x);
    m_direction[1] = simdpp::make_float(0, 0, 0, 0, 0, 0, 0, dir.y);
    m_direction[2] = simdpp::make_float(0, 0, 0, 0, 0, 0, 0, dir.z);
    m_length = simdpp::make_float(0, 0, 0, 0, 0, 0, 0, length);
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
    //failed = simdpp::bit_or(failed, simdpp::cmp_gt(u, oneM256));
    failed = simdpp::bit_or(failed, simdpp::cmp_lt(v, zeroM256));
    failed = simdpp::bit_or(failed, simdpp::cmp_gt(simdpp::add(u, v), oneM256));
    failed = simdpp::bit_or(failed, simdpp::cmp_lt(t, zeroM256));
    failed = simdpp::bit_or(failed, simdpp::cmp_gt(t, m_length));
    failed = simdpp::bit_or(failed, packedTris.inactiveMask);

    const simdpp::float32<width> tResults = simdpp::blend(t, minusOneM256, failed);
    //simdpp::float32<width> temp_mask= simdpp::sign(tResults);
    float temp[width];
    simdpp::store(temp, tResults);
    int mask = 0;
    for (int i=0; i < width; ++i)
    {   
        if(std::signbit(temp[i]))
        {
            mask |= (1<<i);
        }
    }

    if (mask != 0xFF)
    {
        // There is at least one intersection
        result.idx = -1;

        float* ptr = (float*)&tResults;
        for (int i = 0; i < width; ++i)
        {
            if (ptr[i] >= 0.0f && ptr[i] < result.t)
            {
                result.t = ptr[i];
                result.idx = i;
            }
        }

        return result.idx != -1;
    }

    return false;
}

}