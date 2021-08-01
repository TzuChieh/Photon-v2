#include "InterfaceGen/InterfaceGenerator.h"

#include <Common/assertion.h>

#include <utility>

namespace ph::sdlgen
{

InterfaceGenerator::InterfaceGenerator(std::string generatorName, Path outputDirectory) :
	m_generatorName  (std::move(generatorName)),
	m_outputDirectory(std::move(outputDirectory))
{
	PH_ASSERT(m_outputDirectory.isDirectory());
}

Path InterfaceGenerator::makeOutputFilePath(const std::string& fileSubPath) const
{
	return m_outputDirectory.append(Path(fileSubPath));
}

}// end namespace ph::sdlgen
