#include "SIMD_width.h"

// #include "Core/Intersectable/PTriangle.h"


//base on https://stackoverflow.com/questions/45599766/fast-sse-ray-4-triangle-intersection

namespace SIMDPP_ARCH_NAMESPACE {

void print_arch()
{
	std::cout << static_cast<unsigned>(simdpp::this_compile_arch()) << '\n';
}

} // namespace SIMDPP_ARCH_NAMESPACE

SIMDPP_MAKE_DISPATCHER_VOID0(print_arch);

namespace ph
{

constexpr int width = 8;

class testTriangle {
    public:
        Vector3R m_vertex[3];
        testTriangle()
        {

        }
        testTriangle(const Vector3R& a,const Vector3R& b, const Vector3R& c)
        {
            m_vertex[0] = a;
            m_vertex[1] = b;
            m_vertex[2] = c;
        }

        void setVertex(const Vector3R& a,const Vector3R& b, const Vector3R& c)
        {
            m_vertex[0] = a;
            m_vertex[1] = b;
            m_vertex[2] = c;
        }
};

inline bool RayIntersectsTriangle(Vector3R rayOrigin, 
                           Vector3R rayVector, 
                           testTriangle& inTriangle,
                           Vector3R& outIntersectionPoint)
{
    const float EPSILON = 0.0000001;
    Vector3R vertex0 = inTriangle.m_vertex[0];
    Vector3R vertex1 = inTriangle.m_vertex[1];  
    Vector3R vertex2 = inTriangle.m_vertex[2];
    Vector3R edge1, edge2, h, s, q;
    float a,f,u,v;
    edge1 = vertex1 - vertex0;
    edge2 = vertex2 - vertex0;
    h = rayVector.cross(edge2);
    a = edge1.dot(h);
    if (a > -EPSILON && a < EPSILON)
        return false;    // This ray is parallel to this triangle.
    f = 1.0/a;
    s = rayOrigin - vertex0;
    u = f * (s.dot(h));
    if (u < 0.0 || u > 1.0)
        return false;
    q = s.cross(edge1);
    v = f * rayVector.dot(q);
    if (v < 0.0 || u + v > 1.0)
        return false;
    // At this stage we can compute t to find out where the intersection point is on the line.
    float t = f * edge2.dot(q);
    if (t > EPSILON) // ray intersection
    {
        outIntersectionPoint = rayOrigin + rayVector * t;
        return true;
    }
    else // This means that there is a line intersection but not a ray intersection.
        return false;
}

class PackedTriangle {
    
    public:
        //e2 = v2-v0
        //e1 = v1-v0
        std::array< simdpp::float32<width> , 3>  e1;
        std::array< simdpp::float32<width> , 3>  e2;
        std::array< simdpp::float32<width> , 3>  v0;
        simdpp::mask_float32<width> inactiveMask; // Required. We cant always have 8 triangles per packet.
        PackedTriangle(){};
        //this only works when width = 8, and tris.size = 8;
        void setVertex(const testTriangle* tris) 
        {
            this->e1[0] = simdpp::make_float(tris[0].m_vertex[1].x-tris[0].m_vertex[0].x, tris[1].m_vertex[1].x-tris[1].m_vertex[0].x, tris[2].m_vertex[1].x-tris[2].m_vertex[0].x, tris[3].m_vertex[1].x-tris[3].m_vertex[0].x, tris[4].m_vertex[1].x-tris[4].m_vertex[0].x, tris[5].m_vertex[1].x-tris[5].m_vertex[0].x, tris[6].m_vertex[1].x-tris[6].m_vertex[0].x, tris[7].m_vertex[1].x-tris[7].m_vertex[0].x);
            this->e1[1] = simdpp::make_float(tris[0].m_vertex[1].y-tris[0].m_vertex[0].y, tris[1].m_vertex[1].y-tris[1].m_vertex[0].y, tris[2].m_vertex[1].y-tris[2].m_vertex[0].y, tris[3].m_vertex[1].y-tris[3].m_vertex[0].y, tris[4].m_vertex[1].y-tris[4].m_vertex[0].y, tris[5].m_vertex[1].y-tris[5].m_vertex[0].y, tris[6].m_vertex[1].y-tris[6].m_vertex[0].y, tris[7].m_vertex[1].y-tris[7].m_vertex[0].y);
            this->e1[2] = simdpp::make_float(tris[0].m_vertex[1].z-tris[0].m_vertex[0].z, tris[1].m_vertex[1].z-tris[1].m_vertex[0].z, tris[2].m_vertex[1].z-tris[2].m_vertex[0].z, tris[3].m_vertex[1].z-tris[3].m_vertex[0].z, tris[4].m_vertex[1].z-tris[4].m_vertex[0].z, tris[5].m_vertex[1].z-tris[5].m_vertex[0].z, tris[6].m_vertex[1].z-tris[6].m_vertex[0].z, tris[7].m_vertex[1].z-tris[7].m_vertex[0].z);
            
            this->e2[0] = simdpp::make_float(tris[0].m_vertex[2].x-tris[0].m_vertex[0].x, tris[1].m_vertex[2].x-tris[1].m_vertex[0].x, tris[2].m_vertex[2].x-tris[2].m_vertex[0].x, tris[3].m_vertex[2].x-tris[3].m_vertex[0].x, tris[4].m_vertex[2].x-tris[4].m_vertex[0].x, tris[5].m_vertex[2].x-tris[5].m_vertex[0].x, tris[6].m_vertex[2].x-tris[6].m_vertex[0].x, tris[7].m_vertex[2].x-tris[7].m_vertex[0].x);
            this->e2[1] = simdpp::make_float(tris[0].m_vertex[2].y-tris[0].m_vertex[0].y, tris[1].m_vertex[2].y-tris[1].m_vertex[0].y, tris[2].m_vertex[2].y-tris[2].m_vertex[0].y, tris[3].m_vertex[2].y-tris[3].m_vertex[0].y, tris[4].m_vertex[2].y-tris[4].m_vertex[0].y, tris[5].m_vertex[2].y-tris[5].m_vertex[0].y, tris[6].m_vertex[2].y-tris[6].m_vertex[0].y, tris[7].m_vertex[2].y-tris[7].m_vertex[0].y);
            this->e2[2] = simdpp::make_float(tris[0].m_vertex[2].z-tris[0].m_vertex[0].z, tris[1].m_vertex[2].z-tris[1].m_vertex[0].z, tris[2].m_vertex[2].z-tris[2].m_vertex[0].z, tris[3].m_vertex[2].z-tris[3].m_vertex[0].z, tris[4].m_vertex[2].z-tris[4].m_vertex[0].z, tris[5].m_vertex[2].z-tris[5].m_vertex[0].z, tris[6].m_vertex[2].z-tris[6].m_vertex[0].z, tris[7].m_vertex[2].z-tris[7].m_vertex[0].z);

            this->v0[0] = simdpp::make_float(tris[0].m_vertex[0].x, tris[1].m_vertex[0].x, tris[2].m_vertex[0].x, tris[3].m_vertex[0].x, tris[4].m_vertex[0].x, tris[5].m_vertex[0].x, tris[6].m_vertex[0].x, tris[7].m_vertex[0].x);
            this->v0[1] = simdpp::make_float(tris[0].m_vertex[0].y, tris[1].m_vertex[0].y, tris[2].m_vertex[0].y, tris[3].m_vertex[0].y, tris[4].m_vertex[0].y, tris[5].m_vertex[0].y, tris[6].m_vertex[0].y, tris[7].m_vertex[0].y);
            this->v0[2] = simdpp::make_float(tris[0].m_vertex[0].z, tris[1].m_vertex[0].z, tris[2].m_vertex[0].z, tris[3].m_vertex[0].z, tris[4].m_vertex[0].z, tris[5].m_vertex[0].z, tris[6].m_vertex[0].z, tris[7].m_vertex[0].z);
        }
        
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
        std::array< simdpp::float32<width> , 3> m_origin;
        std::array< simdpp::float32<width> , 3> m_direction;
        simdpp::float32<width> m_length;
        bool isIntersectPackedTriangle(const PackedTriangle& triangle, PackedIntersectionResult& result);
        testRay(const Ray& r);  
        
};




template <typename T>
void avx_multi_cross(std::array<T, 3>& result, const std::array<T, 3>& a, const std::array<T, 3>&  b)
{
    result[0] = simdpp::fmsub(a[1], b[2], simdpp::mul(b[1], a[2]));
    result[1] = simdpp::fmsub(a[2], b[0], simdpp::mul(b[2], a[0]));
    result[2] = simdpp::fmsub(a[0], b[1], simdpp::mul(b[0], a[1]));
}

template <typename T>
T avx_multi_dot(const std::array<T, 3>&  a, const std::array<T, 3>&  b)
{
    return simdpp::fmadd(a[2], b[2], simdpp::fmadd(a[1], b[1], simdpp::mul(a[0], b[0])));
}

template <typename T>
void avx_multi_sub(std::array<T, 3>&  result, const std::array<T, 3>&  a, const std::array<T, 3>&  b)
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

bool testRay::isIntersectPackedTriangle(const PackedTriangle& packedTris, PackedIntersectionResult& result)
{
    //must sort the triangles first
    std::array< simdpp::float32<width> , 3> ray_cross_e2;
    avx_multi_cross(ray_cross_e2, m_direction, packedTris.e2);

    simdpp::float32<width> a = avx_multi_dot(packedTris.e1, ray_cross_e2);

    
    simdpp::float32<width> f = simdpp::div(oneM256 , a);

    std::array< simdpp::float32<width> , 3>  s;
    avx_multi_sub(s, m_origin, packedTris.v0);

    simdpp::float32<width> u = simdpp::mul(f, avx_multi_dot(s, ray_cross_e2));

    std::array< simdpp::float32<width> , 3>  q;
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
    
    //simdpp::sign(temp)

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


}


//SIMDPP_MAKE_DISPATCHER(template <typename T> void avx_multi_cross());

