#pragma once

#include <stdexcept>
#include <string>
#include <string_view>
#include <format>
#include <type_traits>
#include <utility>

namespace ph
{

// Note: When adding new base types, the implementation of `CPhotonException` needs to be updated.

// A convenient "catch all" type when handling exceptions
using Exception = std::exception;

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

class OverflowException : public RuntimeException
{
public:
	using RuntimeException::RuntimeException;
};

class UninitializedObjectException : public LogicalException
{
public:
	using LogicalException::LogicalException;
};

template<typename T>
concept CPhotonException = 
	std::is_base_of_v<RuntimeException, T> ||
	std::is_base_of_v<LogicalException, T>;

template<CPhotonException ExceptionType, typename... Args>
inline void throw_formatted(const std::string_view formattedMsg, Args&&... args)
{
	throw ExceptionType(
		std::vformat(formattedMsg, std::make_format_args(std::forward<Args>(args)...)));
}

}// end namespace ph
