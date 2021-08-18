#pragma once

#include <stdexcept>
#include <string>
#include <utility>
#include <format>

namespace ph
{

class IOException : public std::runtime_error
{
public:
	explicit IOException(const std::string& message);
	explicit IOException(const char* message);

	virtual std::string whatStr() const;
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

	return std::format("{}, filename <{}>", IOException::whatStr(), filenameInfo);
}

}// end namespace ph
