#pragma once

#include <stdexcept>
#include <string>
#include <stdexcept>

namespace ph
{

class SdlException : public std::runtime_error
{
public:
	explicit SdlException(const std::string& message);
	explicit SdlException(const char* message);
};

class SdlLoadError : public SdlException
{
public:
	using SdlException::SdlException;
};

class SdlConvertError : public SdlException
{
public:
	using SdlException::SdlException;
};

// In-header Implementations:

inline SdlException::SdlException(const std::string& message) :
	std::runtime_error(message)
{}

inline SdlException::SdlException(const char* const message) :
	std::runtime_error(message)
{}

}// end namespace ph
