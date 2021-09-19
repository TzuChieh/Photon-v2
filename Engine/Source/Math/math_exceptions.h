#pragma once

#include <stdexcept>
#include <string>

namespace ph::math
{

/*! @brief General exception thrown on error related to math functionalities.
*/
class MathException : public std::runtime_error
{
public:
	explicit MathException(const std::string& message);
	explicit MathException(const char* message);

	std::string whatStr() const;
};

/*! @brief Error on the color-related functionalities.
*/
class ColorError : public MathException
{
public:
	using MathException::MathException;
};

// In-header Implementations:

inline MathException::MathException(const std::string& message) :
	std::runtime_error(message)
{}

inline MathException::MathException(const char* const message) :
	std::runtime_error(message)
{}

inline std::string MathException::whatStr() const
{
	return std::string(what());
}

}// end namespace ph::math
