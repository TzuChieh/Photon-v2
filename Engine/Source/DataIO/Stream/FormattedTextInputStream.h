#pragma once

#include "DataIO/Stream/StdInputStream.h"
#include "DataIO/FileSystem/Path.h"

#include <string>
#include <istream>
#include <memory>

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

	std::string acquireName() override;

	/*! @brief Read all text without any whitespaces.
	*/
	void readAllTightly(std::string* out_allText);

	/*! @brief Read a line.
	*/
	void readLine(std::string* out_lineText);

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
