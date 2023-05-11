#pragma once

#include "DataIO/Stream/StdOutputStream.h"
#include "DataIO/FileSystem/Path.h"

#include <string>
#include <string_view>
#include <format>
#include <utility>

namespace ph
{

class FormattedTextOutputStream : public StdOutputStream
{
public:
	inline FormattedTextOutputStream() = default;

	/*! @brief Output to a file as stream.
	*/
	explicit FormattedTextOutputStream(const Path& filePath);

	/*! @brief Output to a string as stream.
	*/
	explicit FormattedTextOutputStream(std::string initialStrContent);

	inline FormattedTextOutputStream(FormattedTextOutputStream&& other) = default;

	std::string acquireName() const override;

	void writeChar(char ch);
	void writeNewLine();

	using StdOutputStream::writeString;

	template<typename... Args>
	void writeString(std::string_view formatStr, Args&&... args);

	/*! @brief Get the string that was written.
	@exception IOException If the stream is not a string stream.
	*/
	std::string getString() const;

	inline FormattedTextOutputStream& operator = (FormattedTextOutputStream&& rhs) = default;

private:
	std::string m_streamName;

	// Unlike text input stream where input data can be pre-stored within the stream object and be
	// accessed polymorphically, text output stream would need separate interface for accessing the 
	// output data. Using this flag so we can support string stream without introducing an extra class.
	bool m_isStringStream;
};

// In-header Implementations:

template<typename... Args>
inline void FormattedTextOutputStream::writeString(const std::string_view formatStr, Args&&... args)
{
	writeString(
		std::vformat(formatStr, std::make_format_args(std::forward<Args>(args)...)));
}

}// end namespace ph
