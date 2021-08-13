#pragma once

#include "InterfaceGen/EInterfaceGenerator.h"

#include <DataIO/FileSystem/Path.h>
#include <Common/assertion.h>

#include <vector>
#include <string>
#include <string_view>
#include <memory>

namespace ph { class SdlClass; }
namespace ph { class SdlEnum; }

namespace ph::sdlgen
{

class InterfaceGenerator
{
public:
	static std::unique_ptr<InterfaceGenerator> makeGenerator(
		EInterfaceGenerator type,
		Path                outputDirectory);

	InterfaceGenerator(std::string generatorName, Path outputDirectory);
	virtual ~InterfaceGenerator() = default;

	virtual void generate(
		const std::vector<const SdlClass*>& sdlClasses,
		const std::vector<const SdlEnum*>&  sdlEnums) = 0;

	const std::string& getName() const;
	const Path& getOutputDirectory() const;
	Path makeOutputFilePath(const std::string& fileSubPath) const;

private:
	std::string m_generatorName;
	Path        m_outputDirectory;
};

// In-header Implementations:

inline const std::string& InterfaceGenerator::getName() const
{
	return m_generatorName;
}

inline const Path& InterfaceGenerator::getOutputDirectory() const
{
	return m_outputDirectory;
}

}// end namespace ph::sdlgen
