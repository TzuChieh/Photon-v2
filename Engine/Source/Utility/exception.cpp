#include "Utility/exception.h"

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

}// end namespace ph
