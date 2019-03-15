#include "Common/config.h"
#include "Core/Ray.h"

#include <simdpp/simd.h>

#include <limits>
#include <iostream>
#include <cmath>
#include <vector>
#include <cstdint>
#include <bitset>

// #include "Core/Intersectable/PTriangle.h"
//#define SIMDPP_ARCH_X86_AVX

//base on https://stackoverflow.com/questions/45599766/fast-sse-ray-4-triangle-intersection

namespace ph
{

constexpr int width = 8;

struct PackedTriangle {
    //e2 = v2-v0
    //e1 = v1-v0
    simdpp::float32<width> e1[3];
    simdpp::float32<width> e2[3];
    simdpp::float32<width> v0[3];
    simdpp::mask_float32<width> inactiveMask; // Required. We cant always have 8 triangles per packet.
};

class PackedIntersectionResult
{
    public:
        float t = std::numeric_limits<float>::infinity();;
        int idx;

};


inline std::ostream& operator<<(std::ostream& os,const PackedIntersectionResult results)
{
    os << "t:" << results.t << " " << "idx:" << results.idx << "\n";
    return os;
}

class testRay 
{
    public:
        simdpp::float32<width> m_origin[3];
        simdpp::float32<width> m_direction[3];
        simdpp::float32<width> m_length;
        bool isIntersectPackedTriangle(const PackedTriangle& triangle, PackedIntersectionResult& result);
        testRay(const Ray& r);
        // void showRay();
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

const simdpp::float32<width> oneM256 = simdpp::splat(1.0f);
const simdpp::float32<width> minusOneM256 = simdpp::splat(-1.0f);
const simdpp::float32<width> positiveEpsilonM256 =  simdpp::splat(1e-6f);;
const simdpp::float32<width> negativeEpsilonM256 = simdpp::splat(-1e-6f);;
const simdpp::float32<width> zeroM256 = simdpp::splat(0.0f);;

}// end namespace ph



