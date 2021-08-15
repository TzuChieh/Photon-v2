#include "InterfaceGen/InterfaceGenerator.h"
#include "InterfaceGen/MarkdownDoc/MarkdownDocGenerator.h"

#include <Common/assertion.h>

#include <utility>

namespace ph::sdlgen
{

std::unique_ptr<InterfaceGenerator> InterfaceGenerator::makeGenerator(
	const EInterfaceGenerator type,
	Path                      outputDirectory)
{
	switch(type)
	{
	case EInterfaceGenerator::MARKDOWN_DOC:
		return std::make_unique<MarkdownDocGenerator>(std::move(outputDirectory));

	default:
		return nullptr;
	}
}

InterfaceGenerator::InterfaceGenerator(std::string generatorName, Path outputDirectory) :
	m_generatorName  (std::move(generatorName)),
	m_outputDirectory(std::move(outputDirectory))
{
	PH_ASSERT(m_outputDirectory.hasDirectory());
}

Path InterfaceGenerator::makeOutputFilePath(const std::string& fileSubPath) const
{
	return m_outputDirectory.append(Path(fileSubPath));
}

}// end namespace ph::sdlgen
