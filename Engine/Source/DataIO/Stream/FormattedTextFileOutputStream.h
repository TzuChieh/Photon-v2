#pragma once

#include "DataIO/Stream/StdOutputStream.h"
#include "DataIO/FileSystem/Path.h"

#include <string_view>

namespace ph
{

class FormattedTextFileOutputStream : public StdOutputStream
{
public:
	inline FormattedTextFileOutputStream() = default;
	explicit FormattedTextFileOutputStream(const Path& filePath);
	inline FormattedTextFileOutputStream(FormattedTextFileOutputStream&& other) = default;

	bool writeStr(std::string_view str);

	inline FormattedTextFileOutputStream& operator = (FormattedTextFileOutputStream&& rhs) = default;
};

}// end namespace ph
