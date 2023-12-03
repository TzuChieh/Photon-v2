#pragma once

#include "Utility/exception.h"

#include <stdexcept>
#include <string>

namespace ph::math
{

/*! @brief General exception thrown on error related to math functionalities.
*/
class MathException : public RuntimeException
{
public:
	using RuntimeException::RuntimeException;

	std::string whatStr() const override;
};

/*! @brief Error on the color-related functionalities.
*/
class ColorError : public MathException
{
public:
	using MathException::MathException;
};

// In-header Implementations:

inline std::string MathException::whatStr() const
{
	return std::string(what());
}

}// end namespace ph::math
