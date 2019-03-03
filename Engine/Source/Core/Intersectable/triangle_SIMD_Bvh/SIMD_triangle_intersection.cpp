#include "simdpp/simd.h"
#include <limits>
#include <iostream>
#include <cmath>
// #include "Core/Ray.h"
// #include "Core/Intersectable/PTriangle.h"
//#define SIMDPP_ARCH_X86_AVX

//base on https://stackoverflow.com/questions/45599766/fast-sse-ray-4-triangle-intersection

namespace ph
{

constexpr int width = 8;

struct PackedTriangle {
    simdpp::float32<width> e1[3];
    simdpp::float32<width> e2[3];
    simdpp::float32<width> v0[3];
    simdpp::float32<width> inactiveMask; // Required. We cant always have 8 triangles per packet.
};

struct PackedIntersectionResult
{
    float t = std::numeric_limits<float>::infinity();;
    int idx;
};

class PackedRay 
{
    public:
        simdpp::float32<width> m_origin[3];
        simdpp::float32<width> m_direction[3];
        simdpp::float32<width> m_length;
        bool isIntersectPackedTriangle(const PackedTriangle& triangle, PackedIntersectionResult& result);
};



template <typename T>
void avx_multi_cross(T result[3], const T a[3], const T b[3])
{
    result[0] = simdpp::fmsub(a[1], b[2], simdpp::mul(b[1], a[2]));
    result[1] = simdpp::fmsub(a[2], b[0], simdpp::mul(b[2], a[0]));
    result[2] = simdpp::fmsub(a[0], b[1], simdpp::mul(b[0], a[1]));
}

template <typename T>
T avx_multi_dot(const T a[3], const T b[3])
{
    return simdpp::fmadd(a[2], b[2], simdpp::fmadd(a[1], b[1], simdpp::mul(a[0], b[0])));
}

template <typename T>
void avx_multi_sub(T result[3], const T a[3], const T b[3])
{
    result[0] = simdpp::sub(a[0], b[0]);
    result[1] = simdpp::sub(a[1], b[1]);
    result[2] = simdpp::sub(a[2], b[2]);
}

const simdpp::float32<width> oneM256 = simdpp::splat(1.0);
const simdpp::float32<width> minusOneM256 = simdpp::splat(-1.0f);
const simdpp::float32<width> positiveEpsilonM256 =  simdpp::splat(1e-6f);;
const simdpp::float32<width> negativeEpsilonM256 = simdpp::splat(-1e-6f);;
const simdpp::float32<width> zeroM256 = simdpp::splat(0.0f);;

bool PackedRay::isIntersectPackedTriangle(const PackedTriangle& packedTris, PackedIntersectionResult& result)
{
    simdpp::float32<width> q[3];
    avx_multi_cross(q, m_direction, packedTris.e2);

    simdpp::float32<width> a = avx_multi_dot(packedTris.e1, q);

    simdpp::float32<width> f = simdpp::div(oneM256 , a);

    simdpp::float32<width> s[3];
    avx_multi_sub(s, m_origin, packedTris.v0);

    simdpp::float32<width> u = simdpp::mul(f, avx_multi_dot(s, q));

    simdpp::float32<width> r[3];
    avx_multi_cross(r, s, packedTris.e1);

    simdpp::float32<width> v = simdpp::mul(f, avx_multi_dot(m_direction, r));

    simdpp::float32<width> t = simdpp::mul(f, avx_multi_dot(packedTris.e2, r));

    // Failure conditions
    simdpp::float32<width> failed = simdpp::bit_and(
        simdpp::bit_cast<simdpp::float32<width>>(simdpp::cmp_gt(a, negativeEpsilonM256)) ,
        simdpp::cmp_lt(a, positiveEpsilonM256)
    );
    
    //simdpp::float32<width> failed;
    failed = simdpp::bit_or(failed, simdpp::cmp_lt(u, zeroM256));
    failed = simdpp::bit_or(failed, simdpp::cmp_lt(v, zeroM256));
    failed = simdpp::bit_or(failed, simdpp::cmp_gt(simdpp::add(u, v), oneM256));
    failed = simdpp::bit_or(failed, simdpp::cmp_lt(t, zeroM256));
    failed = simdpp::bit_or(failed, simdpp::cmp_gt(t, m_length));
    failed = simdpp::bit_or(failed, packedTris.inactiveMask);

    const simdpp::float32<width> tResults = simdpp::blend(t, minusOneM256, failed);
    //std::cout << tResults << std::endl;
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
    //float mask = simdpp::sign(tResults);

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

