#pragma once

#include "InterfaceGen/InterfaceGenerator.h"

namespace ph::sdlgen
{

class MarkdownDocGenerator : public InterfaceGenerator
{
public:
	MarkdownDocGenerator(std::string generatorName, Path outputDirectory);

	void generate(
		const std::vector<const SdlClass*>& sdlClasses,
		const std::vector<const SdlEnum*>&  sdlEnums) const override;

private:
};

}// end namespace ph::sdlgen
