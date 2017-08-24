#pragma once

#include "Common/primitive_type.h"
#include "Math/TVector3.h"

namespace ph
{

class Microfacet
{
public:
	virtual ~Microfacet() = 0;

	virtual real distribution(const Vector3R& N, const Vector3R& H) const = 0;
	virtual real shadowing(const Vector3R& N, const Vector3R& H, 
	                       const Vector3R& L, const Vector3R& V) const = 0;
	virtual void genDistributedH(real seedA_i0e1, real seedB_i0e1, 
	                             const Vector3R& N, Vector3R* out_H) const = 0;
};

}// end namespace ph