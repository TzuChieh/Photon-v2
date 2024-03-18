#include "Common/exceptions.h"

namespace ph
{

RuntimeException::RuntimeException(const std::string& message) :
	std::runtime_error(message)
{}

RuntimeException::RuntimeException(const char* const message) :
	std::runtime_error(message)
{}

std::string RuntimeException::whatStr() const
{
	return std::string(what());
}

LogicalException::LogicalException(const std::string& message) :
	std::logic_error(message)
{}

LogicalException::LogicalException(const char* const message) :
	std::logic_error(message)
{}

std::string LogicalException::whatStr() const
{
	return std::string(what());
}

}// end namespace ph
