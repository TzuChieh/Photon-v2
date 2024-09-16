#pragma once

#include "DataIO/Stream/StdInputStream.h"
#include "DataIO/FileSystem/Path.h"

#include <string>
#include <istream>
#include <memory>

namespace ph
{

/*! @brief Read text with basic auto formatting applied.
*/
class FormattedTextInputStream : public StdInputStream
{
public:
	inline FormattedTextInputStream() = default;

	/*! @brief Take a file as stream.
	@param textFilePath The file to open as stream.
	*/
	explicit FormattedTextInputStream(const Path& textFilePath);

	/*! @brief Take a string as stream.
	@param textString The string to read as stream. This string will be copied.
	*/
	explicit FormattedTextInputStream(std::string textString);

	inline FormattedTextInputStream(FormattedTextInputStream&& other) = default;

	std::string acquireName() const override;

	/*! @brief Read all remaining text. All contents are preserved.
	All original input contents are preserved (including any line endings such as CRLF, LF), no matter
	what platform the stream is operating on.
	@param[out] out_allText Buffer to store all text from this stream.
	*/
	void readAll(std::string* out_allText);

	/*! @brief Read all remaining text without any whitespaces.
	@param[out] out_allText Buffer to store all text from this stream with whitespaces removed.
	*/
	void readAllTightly(std::string* out_allText);

	// TODO: readLineTightly()?

	inline FormattedTextInputStream& operator = (FormattedTextInputStream&& rhs) = default;

protected:
	/*!
	@param stream A standard formatted input stream.
	*/
	FormattedTextInputStream(std::unique_ptr<std::istream> stream, std::string streamName);

private:
	std::string m_streamName;
};

}// end namespace ph
