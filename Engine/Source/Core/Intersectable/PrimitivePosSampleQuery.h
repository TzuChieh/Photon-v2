#pragma once

#include "Common/primitive_type.h"
#include "Common/assertion.h"
#include "Math/TVector3.h"

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
	real           pdfA;

	PrimitivePosSampleOutput();
	operator bool () const;
};

class PrimitivePosSampleQuery final
{
public:
	using Input  = PrimitivePosSampleInput;
	using Output = PrimitivePosSampleOutput;

	Input  inputs;
	Output outputs;

	PrimitivePosSampleQuery();
};

// In-header Implementations:

inline PrimitivePosSampleOutput::PrimitivePosSampleOutput() :
	position(0),
	normal  (0, 1, 0),
	uvw     (0),
	pdfA    (0)
{
	PH_ASSERT(!(*this));
}

inline PrimitivePosSampleOutput::operator bool () const
{
	PH_ASSERT_IN_RANGE(normal.length(), 0.9_r, 1.1_r);
	return pdfA > 0;
}

inline PrimitivePosSampleQuery::PrimitivePosSampleQuery() :
	inputs (),
	outputs()
{}

}// end namespace ph
