#pragma once

#include "Core/IntersectionDetail.h"
#include "Math/TVector3.h"

namespace ph
{

class UvwMapper
{
public:
	virtual ~UvwMapper() = 0;

	virtual void map(const IntersectionDetail& X, Vector3R* out_uvw) const = 0;
};

}// end namespace ph