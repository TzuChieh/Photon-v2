#pragma once

#include <stdexcept>
#include <string>

namespace ph
{

class IOException : public std::runtime_error
{
public:
	explicit IOException(const std::string& message);
	explicit IOException(const char* message);
};

class FileIOError : public IOException
{
public:
	using IOException::IOException;
};

// In-header Implementations:

inline IOException::IOException(const std::string& message) :
	std::runtime_error(message)
{}

inline IOException::IOException(const char* const message) :
	std::runtime_error(message)
{}

}// end namespace ph
