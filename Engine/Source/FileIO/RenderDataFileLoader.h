#pragma once

#include "Actor/AModel.h"

#include <vector>
#include <string>
#include <fstream>

namespace ph
{

class RenderDataFileLoader final
{
public:
	RenderDataFileLoader();

	bool load(const std::string& fullFilename);
	std::vector<AModel> getLoadedModels() const;

private:
	std::vector<AModel> m_loadedModels;

	void clearLoadedData();
	void parseDataFile(std::ifstream* const dataFile);
};

}// end namespace ph