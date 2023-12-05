#pragma once

#include "Math/TVector3.h"

#include <Common/primitive_type.h>

#include <cstddef>

namespace ph { class HitDetail; }
namespace ph { class PrimitivePosSampleQuery; }

namespace ph
{

class PrimitivePosSamplePdfInput final
{
public:
	math::Vector3R position;
	std::size_t    faceId;

	PrimitivePosSamplePdfInput();

	void set(const math::Vector3R& position);
	void set(const math::Vector3R& position, std::size_t faceId);
	void set(const HitDetail& detail);
	void set(const PrimitivePosSampleQuery& query);
};

class PrimitivePosSamplePdfOutput final
{
public:
	real pdfA;

	PrimitivePosSamplePdfOutput();
	operator bool () const;
};

class PrimitivePosSamplePdfQuery final
{
public:
	using Input  = PrimitivePosSamplePdfInput;
	using Output = PrimitivePosSamplePdfOutput;

	Input  in;
	Output out;

	PrimitivePosSamplePdfQuery();
};

// In-header Implementations:

inline PrimitivePosSamplePdfOutput::PrimitivePosSamplePdfOutput() :
	pdfA(0)
{}

inline PrimitivePosSamplePdfOutput::operator bool () const
{
	return pdfA > 0;
}

inline PrimitivePosSamplePdfQuery::PrimitivePosSamplePdfQuery() :
	in(), out()
{}

}// end namespace ph
