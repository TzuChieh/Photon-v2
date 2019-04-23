#include "SIMD_width.h"
#include <iostream>
#include "Source/Math/constant.h"
// inline constexpr AxisIndexType X_AXIS       = 0;
// inline constexpr AxisIndexType Y_AXIS       = 1;
// inline constexpr AxisIndexType Z_AXIS       = 2;
namespace ph
{
    
class SIMD_BVH_Node
{
    public:
        // a width wide boxes with min-max[2] , xyz[3]
        simdpp::float32<width> bboxes[2][3];
        // 4 children
        int children[width];
        //top axis, left axis , right axis
        int axis0, axis1, axis2;
        int fill;
        bool isIntersecting();
};
bool SIMD_BVH_Node::isIntersecting(
    const simdpp::float32<width> bboxes[2][3],
    const simdpp::float32<width> ray_origin[3],
    const simdpp::float32<width> idir[3],
    const int sign[3],
    simdpp::float32<width> tmin,
    simdpp::float32<width> tmax

)
{   
    //TODO
    //read ifconstexpr

    //x coordinate
    tmin = simdpp::max( tmin, simdpp::mul(simdpp::sub( bboxes[sign[0][0] ][ ray_origin[0] ] ) , idir[0]);
    tmax = simdpp::min( tmax, simdpp::mul(simdpp::sub( bboxes[ 1 - sign[0][0] ][ ray_origin[0] ] ) , idir[0]);

    //y cooridinate
    tmin = simdpp::max( tmin, simdpp::mul(simdpp::sub( bboxes[sign[1][1] ][ ray_origin[1] ] ) , idir[1]);
    tmax = simdpp::min( tmax, simdpp::mul(simdpp::sub( bboxes[ 1 - sign[1][1] ][ ray_origin[1] ] ) , idir[1]);

    //z coordinate
    tmin = simdpp::max( tmin, simdpp::mul(simdpp::sub( bboxes[sign[2][2] ][ ray_origin[2] ] ) , idir[2]);
    tmax = simdpp::min( tmax, simdpp::mul(simdpp::sub( bboxes[ 1 - sign[2][2] ][ ray_origin[2] ] ) , idir[2]);

    return simdpp::sign( simdpp::cmp_gt(tmax, tmin) );
}

}
