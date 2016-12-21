#pragma once

#include "Entity/Entity.h"

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
	std::vector<Entity> getLoadedEntities() const;

private:
	std::vector<Entity> m_loadedEntities;

	void clearLoadedData();
	void parseDataFile(std::ifstream* const dataFile);
};

}// end namespace ph