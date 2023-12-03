#pragma once

#include "Common/primitive_type.h"
#include "Common/assertion.h"
#include "Math/TVector3.h"

#include <cstddef>

namespace ph
{

class PrimitivePosSampleInput final
{};

class PrimitivePosSampleOutput final
{
public:
	math::Vector3R position;
	math::Vector3R normal;
	math::Vector3R uvw;
	std::size_t    faceId;
	real           pdfA;

	PrimitivePosSampleOutput();
	operator bool () const;
};

class PrimitivePosSampleQuery final
{
public:
	using Input  = PrimitivePosSampleInput;
	using Output = PrimitivePosSampleOutput;

	Input  in;
	Output out;

	PrimitivePosSampleQuery();

	void setInvalidOutput();
};

// In-header Implementations:

inline PrimitivePosSampleOutput::operator bool () const
{
	PH_ASSERT_IN_RANGE(normal.length(), 0.9_r, 1.1_r);
	return pdfA > 0;
}

inline PrimitivePosSampleQuery::PrimitivePosSampleQuery() :
	in(), out()
{}

inline void PrimitivePosSampleQuery::setInvalidOutput()
{
	// Must cuase `PrimitivePosSampleOutput::operator bool ()` method to evaluate to `false`
	out.pdfA = 0.0_r;
}

}// end namespace ph
