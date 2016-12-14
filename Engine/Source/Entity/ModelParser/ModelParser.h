#pragma once

#include <string>

namespace ph
{

class Entity;

class ModelParser
{
public:
	virtual ~ModelParser() = 0;

	virtual bool parse(const std::string& fullFilename, Entity* const out_entity) = 0;
};

}// end namespace ph