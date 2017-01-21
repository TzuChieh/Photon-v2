#pragma once

#include "Actor/AModel.h"

#include <vector>
#include <string>
#include <fstream>

namespace ph
{

class RenderingDescription;

class RenderingFileLoader final
{
public:
	RenderingFileLoader();

	bool load(const std::string& fullFilename, RenderingDescription* const out_data);

private:

	void parseCoreCommands(std::ifstream& dataFile);
	void parseWorldCommands(std::ifstream& dataFile);
};

}// end namespace ph