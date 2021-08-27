#pragma once

#include <stdexcept>
#include <string>

namespace ph
{

/*! @brief General exception thrown on error related to SDL.
*/
class SdlException : public std::runtime_error
{
public:
	explicit SdlException(const std::string& message);
	explicit SdlException(const char* message);

	std::string whatStr() const;
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
	std::runtime_error(message)
{}

inline SdlException::SdlException(const char* const message) :
	std::runtime_error(message)
{}

inline std::string SdlException::whatStr() const
{
	return std::string(what());
}

}// end namespace ph
