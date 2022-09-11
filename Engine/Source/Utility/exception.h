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

class LogicalException : public std::logic_error
{
public:
	explicit LogicalException(const std::string& message);
	explicit LogicalException(const char* message);
	inline ~LogicalException() override = default;

	virtual std::string whatStr() const;
};

class UninitializedObjectException : public LogicalException
{
public:
	using LogicalException::LogicalException;
};

}// end namespace ph
