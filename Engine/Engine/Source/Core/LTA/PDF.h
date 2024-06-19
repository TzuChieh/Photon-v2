#pragma once

#include "Core/LTA/enums.h"

#include <Common/assertion.h>
#include <Common/primitive_type.h>

namespace ph::lta
{

/*! @brief A sample from a Probability Density Function (PDF).
*/
class PDF final
{
public:
	/*! Sample value of the PDF.
	*/
	real value = 0.0_r;

	/*! The domain on which the PDF is defined. Integrating the PDF over this domain
	will yield 1.
	*/
	EDomain domain = EDomain::Empty;

	/*!
	@return Solid angle  PDF with `value == pdfW`.
	*/
	static PDF W(real pdfW);

	/*!
	@return Area domain PDF with `value == pdfA`.
	*/
	static PDF A(real pdfA);

	/*!
	@return Discrete domain PDF with `value == pdfD`.
	*/
	static PDF D(real pdfD);

	real getPdfW() const;
	real getPdfA() const;
	real getPdfD() const;

	/*!
	@return Is the PDF in empty domain. Use this method to check whether the PDF is invalid.
	@note It does not mean `value == 0` if `isEmpty() == true`. This method is a convenient helper
	for checking the PDF's domain.
	*/
	bool isEmpty() const;

	/*!
	Convenient method for accessing `value`. It is an error to get the value if `domain` is empty.
	@return `value`.
	*/
	real operator * () const;

	/*! @brief Whether the PDF is sane and positive.
	@return Same as `!isEmpty() && std::isfinite(value) && value > 0`.
	*/
	operator bool () const;

	/*! @brief Perform arithmetic on `value` without changing its domain.
	*/
	///@{
	PDF operator + (real rhs) const;
	PDF operator - (real rhs) const;
	PDF operator * (real rhs) const;
	PDF operator / (real rhs) const;
	///@}
};

inline PDF PDF::W(const real pdfW)
{
	return {.value = pdfW, .domain = EDomain::SolidAngle};
}

inline PDF PDF::A(const real pdfA)
{
	return {.value = pdfA, .domain = EDomain::Area};
}

inline PDF PDF::D(const real pdfD)
{
	return {.value = pdfD, .domain = EDomain::Discrete};
}

inline real PDF::getPdfW() const
{
	PH_ASSERT(domain == EDomain::SolidAngle);
	return value;
}

inline real PDF::getPdfA() const
{
	PH_ASSERT(domain == EDomain::Area);
	return value;
}

inline real PDF::getPdfD() const
{
	PH_ASSERT(domain == EDomain::Discrete);
	return value;
}

inline bool PDF::isEmpty() const
{
	return domain == EDomain::Empty;
}

inline real PDF::operator * () const
{
	PH_ASSERT(!isEmpty());
	return value;
}

inline PDF::operator bool () const
{
	return !isEmpty() && std::isfinite(value) && value > 0;
}

inline PDF PDF::operator + (const real rhs) const
{
	PH_ASSERT(!isEmpty());
	return {.value = value + rhs, .domain = domain};
}

inline PDF PDF::operator - (const real rhs) const
{
	PH_ASSERT(!isEmpty());
	return {.value = value - rhs, .domain = domain};
}

inline PDF PDF::operator * (const real rhs) const
{
	PH_ASSERT(!isEmpty());
	return {.value = value * rhs, .domain = domain};
}

inline PDF PDF::operator / (const real rhs) const
{
	PH_ASSERT(!isEmpty());
	return {.value = value / rhs, .domain = domain};
}

}// end namespace ph::lta
