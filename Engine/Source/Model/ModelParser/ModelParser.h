#pragma once

#include <string>

namespace ph
{

class Model;

class ModelParser
{
public:
	virtual ~ModelParser() = 0;

	virtual bool parse(const std::string& fullFilename, Model* const out_model) = 0;
};

}// end namespace ph