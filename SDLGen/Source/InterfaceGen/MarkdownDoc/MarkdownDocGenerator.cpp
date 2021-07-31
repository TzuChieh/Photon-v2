#include "InterfaceGen/MarkdownDoc/MarkdownDocGenerator.h"

#include <utility>

namespace ph::sdlgen
{

MarkdownDocGenerator::MarkdownDocGenerator(std::string generatorName, Path outputDirectory) :
	InterfaceGenerator(std::move(generatorName), std::move(outputDirectory))
{}

void MarkdownDocGenerator::generate(
	const std::vector<const SdlClass*>& sdlClasses,
	const std::vector<const SdlEnum*>&  sdlEnums) const
{

}

}// end namespace ph::sdlgen
