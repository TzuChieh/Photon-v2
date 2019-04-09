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
    return true;
}

}
