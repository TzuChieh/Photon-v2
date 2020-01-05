#pragma once

#include "DataIO/FileSystem/Path.h"

#include <string>

namespace ph
{

class TextFileReader final
{
public:
	explicit TextFileReader(Path path);

	bool readAll(std::string* out_allText);
	bool readLine(std::string* out_lineText);

private:
	Path m_path;
};

}// end namespace ph
