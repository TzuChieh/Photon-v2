#pragma once

#include <string>
#include <vector>

namespace ph
{

class AModel;

class ModelParser
{
public:
	virtual ~ModelParser() = 0;

	virtual bool parse(const std::string& fullFilename, std::vector<AModel>* const out_models) = 0;
};

}// end namespace ph