#pragma once

#include "Common/primitive_type.h"

#define PH_PI      3.141592653589793238462643383279
#define PH_RECI_PI (1.0 / PH_PI)

#define PH_PI_REAL        static_cast<real>(PH_PI)
#define PH_RECI_PI_REAL   static_cast<real>(PH_RECI_PI)
#define PH_4_PI_REAL      (4.0_r * PH_PI_REAL)
#define PH_RECI_4_PI_REAL (0.25_r * PH_RECI_PI_REAL)

#define PH_SQRT_2           1.414213562373095048801688724209698078569671875376948073176
#define PH_RECI_SQRT_2      0.707106781186547524400844362104849039284835937688474036588
#define PH_SQRT_2_REAL      static_cast<real>(PH_SQRT_2)
#define PH_RECI_SQRT_2_REAL static_cast<real>(PH_RECI_SQRT_2)