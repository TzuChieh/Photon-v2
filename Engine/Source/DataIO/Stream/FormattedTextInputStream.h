#pragma once

#include "DataIO/Stream/IInputStream.h"
#include "DataIO/FileSystem/Path.h"

#include <string>
#include <istream>

namespace ph
{

class FormattedTextInputStream : public IInputStream
{
public:
	explicit FormattedTextInputStream(const Path& textFilePath);
	explicit FormattedTextInputStream(const std::string& textString);

	bool read(std::size_t numBytes, std::byte* out_bytes) override;

	void seekGet(std::size_t pos) override;
	std::size_t tellGet() const override;

	void readAll(std::string* out_allText);
	bool readLine(std::string* out_lineText);

private:
	std::unique_ptr<std::istream> m_istream;
};

}// end namespace ph
