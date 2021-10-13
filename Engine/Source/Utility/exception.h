#pragma once

#include <stdexcept>
#include <string>

namespace ph
{

/*! @brief General exception thrown on runtime error.
*/
class RuntimeException : public std::runtime_error
{
public:
	explicit RuntimeException(const std::string& message);
	explicit RuntimeException(const char* message);
	inline ~RuntimeException() override = default;

	virtual std::string whatStr() const;
};

}// end namespace ph
