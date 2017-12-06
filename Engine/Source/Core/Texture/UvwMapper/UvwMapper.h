#pragma once

#include "Core/HitDetail.h"
#include "Math/TVector3.h"

namespace ph
{

class UvwMapper
{
public:
	virtual ~UvwMapper() = 0;

	virtual void map(const HitDetail& X, Vector3R* out_uvw) const = 0;
};

}// end namespace ph