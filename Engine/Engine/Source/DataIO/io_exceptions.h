#pragma once

#include "Utility/exception.h"

#include <string>
#include <utility>
#include <format>
#include <system_error>

namespace ph
{

class IOException : public RuntimeException
{
public:
	explicit IOException(const std::string& message);
	explicit IOException(const char* message);
};

class FileIOError : public IOException
{
public:
	explicit FileIOError(const std::string& message);
	explicit FileIOError(const char* message);
	FileIOError(const std::string& message, std::string filename);

	std::string whatStr() const override;

private:
	std::string m_filename;
};

class FilesystemError : public IOException
{
public:
	using IOException::IOException;

	explicit FilesystemError(std::error_code errorCode);
	FilesystemError(const std::string& message, std::error_code errorCode);

	std::string whatStr() const override;

private:
	std::error_code m_errorCode;
};

// In-header Implementations:

inline IOException::IOException(const std::string& message) :
	RuntimeException(message)
{}

inline IOException::IOException(const char* const message) :
	RuntimeException(message)
{}

inline FileIOError::FileIOError(const std::string& message) :
	IOException(message),
	m_filename()
{}

inline FileIOError::FileIOError(const char* const message) :
	IOException(message),
	m_filename()
{}

inline FileIOError::FileIOError(const std::string& message, std::string filename) :
	IOException(message),
	m_filename(std::move(filename))
{}

inline std::string FileIOError::whatStr() const
{
	std::string filenameInfo = m_filename.empty() ? "(unavailable)" : m_filename;

	return std::format("{} | filename <{}>", IOException::whatStr(), filenameInfo);
}

inline FilesystemError::FilesystemError(std::error_code errorCode)
	: FilesystemError("", errorCode)
{}

inline FilesystemError::FilesystemError(const std::string& message, std::error_code errorCode)
	: IOException(message)
	, m_errorCode(errorCode)
{}

inline std::string FilesystemError::whatStr() const
{
	auto baseMsg = IOException::whatStr();
	auto errorCodeMsg = m_errorCode.message();
	if(errorCodeMsg.empty())
	{
		return baseMsg;
	}
	else
	{
		return std::format("{} ({})", baseMsg, errorCodeMsg);
	}
}

}// end namespace ph
