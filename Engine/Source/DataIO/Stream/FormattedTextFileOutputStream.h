#pragma once

#include "DataIO/Stream/StdOutputStream.h"
#include "DataIO/FileSystem/Path.h"

#include <string_view>
#include <format>
#include <utility>

namespace ph
{

class FormattedTextFileOutputStream : public StdOutputStream
{
public:
	inline FormattedTextFileOutputStream() = default;
	explicit FormattedTextFileOutputStream(const Path& filePath);
	inline FormattedTextFileOutputStream(FormattedTextFileOutputStream&& other) = default;

	std::string acquireName() override;

	void writeLine(std::string_view line);
	void writeString(std::string_view str);
	void writeChar(char ch);
	void writeNewLine();

	template<typename... Args>
	void write(Args&&... args);

	inline FormattedTextFileOutputStream& operator = (FormattedTextFileOutputStream&& rhs) = default;

private:
	Path m_filePath;
};

// In-header Implementations:

template<typename... Args>
inline void FormattedTextFileOutputStream::write(Args&&... args)
{
	writeString(
		std::format(std::format<Args>(args)...));
}

}// end namespace ph
