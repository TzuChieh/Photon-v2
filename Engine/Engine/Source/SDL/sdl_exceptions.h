#pragma once

#include <Common/exception.h>

#include <string>

namespace ph
{

/*! @brief General exception thrown on error related to SDL.
*/
class SdlException : public RuntimeException
{
public:
	explicit SdlException(const std::string& message);
	explicit SdlException(const char* message);
};

/*! @brief Error on the SDL input process.
*/
class SdlLoadError : public SdlException
{
public:
	using SdlException::SdlException;
};

/*! @brief Error on the SDL output process.
*/
class SdlSaveError : public SdlException
{
public:
	using SdlException::SdlException;
};

// In-header Implementations:

inline SdlException::SdlException(const std::string& message) :
	RuntimeException(message)
{}

inline SdlException::SdlException(const char* const message) :
	RuntimeException(message)
{}

}// end namespace ph
