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

	std::string whatStr() const;
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

inline std::string IOException::whatStr() const
{
	return std::string(what());
}

}// end namespace ph
