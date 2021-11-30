#pragma once

#include "DataIO/Stream/StdInputStream.h"
#include "DataIO/FileSystem/Path.h"

#include <string>

namespace ph
{

class FormattedTextInputStream : public StdInputStream
{
public:
	inline FormattedTextInputStream() = default;

	/*! @brief Take a file as stream.
	*/
	explicit FormattedTextInputStream(const Path& textFilePath);

	/*! @brief Take a string as stream.
	*/
	explicit FormattedTextInputStream(const std::string& textString);

	inline FormattedTextInputStream(FormattedTextInputStream&& other) = default;

	/*! @brief Read all text without any whitespaces.
	*/
	void readAllTightly(std::string* out_allText);

	/*! @brief Read a line.
	*/
	void readLine(std::string* out_lineText);

	/*! @brief Read a line without leading and trailing whitespaces.
	*/
	void readTrimmedLine(std::string* out_lineText);

	// TODO: readLineTightly()?

	inline FormattedTextInputStream& operator = (FormattedTextInputStream&& rhs) = default;
};

}// end namespace ph
