#pragma once

#include <string>
#include <vector>

namespace ph
{

class Model;

class ModelParser
{
public:
	virtual ~ModelParser() = 0;

	virtual bool parse(const std::string& fullFilename, std::vector<Model>* const out_models) = 0;
};

}// end namespace ph