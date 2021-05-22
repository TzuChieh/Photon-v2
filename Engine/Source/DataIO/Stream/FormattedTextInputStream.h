#pragma once

#include "DataIO/Stream/IInputStream.h"
#include "DataIO/FileSystem/Path.h"

#include <utility>
#include <string>
#include <istream>

namespace ph
{

class FormattedTextInputStream : public IInputStream
{
public:
	FormattedTextInputStream() = default;

	/*! @brief Take a file as stream.
	*/
	explicit FormattedTextInputStream(const Path& textFilePath);

	/*! @brief Take a string as stream.
	*/
	explicit FormattedTextInputStream(const std::string& textString);

	FormattedTextInputStream(FormattedTextInputStream&& other);

	bool read(std::size_t numBytes, std::byte* out_bytes) override;
	void seekGet(std::size_t pos) override;
	std::size_t tellGet() override;
	operator bool () const override;

	/*! @brief Read all text without any whitespaces.
	*/
	void readAllTightly(std::string* out_allText);

	/*! @brief Read a line without leading and trailing whitespaces.
	*/
	bool readTrimmedLine(std::string* out_lineText);

	// TODO: readLineTightly()?

	FormattedTextInputStream& operator = (FormattedTextInputStream&& rhs);

private:
	std::unique_ptr<std::istream> m_istream;
};

// In-header Implementations:

inline FormattedTextInputStream::FormattedTextInputStream(FormattedTextInputStream&& other)
{
	*this = std::move(other);
}

inline FormattedTextInputStream& FormattedTextInputStream::operator = (FormattedTextInputStream&& rhs)
{
	m_istream = std::move(rhs.m_istream);
}

inline FormattedTextInputStream::operator bool () const
{
	return m_istream != nullptr && m_istream->good();
}

}// end namespace ph
