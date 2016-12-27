#pragma once

#include "Actor/Model/Model.h"

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
	std::vector<Model> getLoadedModels() const;

private:
	std::vector<Model> m_loadedModels;

	void clearLoadedData();
	void parseDataFile(std::ifstream* const dataFile);
};

}// end namespace ph