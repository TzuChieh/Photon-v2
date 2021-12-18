#pragma once

#include "Math/TVector3.h"
#include "Math/Color/Spectrum.h"

namespace ph
{

class Primitive;

class DirectEnergySampleInput final
{
public:
	math::Vector3R targetPos;

	DirectEnergySampleInput();

	void set(const math::Vector3R& targetPos);
};

class DirectEnergySampleOutput final
{
public:
	math::Vector3R   emitPos;
	math::Spectrum   radianceLe;
	real             pdfW;
	const Primitive* srcPrimitive;

	DirectEnergySampleOutput();
	operator bool () const;
};

class DirectEnergySampleQuery final
{
public:
	using Input  = DirectEnergySampleInput;
	using Output = DirectEnergySampleOutput;

	Input  in;
	Output out;

	DirectEnergySampleQuery();
};

}// end namespace ph
