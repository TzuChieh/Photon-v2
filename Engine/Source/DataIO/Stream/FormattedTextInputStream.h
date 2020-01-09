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
	explicit FormattedTextInputStream(const Path& textFilePath);
	explicit FormattedTextInputStream(const std::string& textString);
	FormattedTextInputStream(FormattedTextInputStream&& other);

	bool read(std::size_t numBytes, std::byte* out_bytes) override;
	void seekGet(std::size_t pos) override;
	std::size_t tellGet() const override;
	operator bool () const override;

	void readAllTightly(std::string* out_allText);
	bool readLineTightly(std::string* out_lineText);

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
