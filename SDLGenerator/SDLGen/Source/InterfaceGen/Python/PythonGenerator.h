#pragma once

#include "InterfaceGen/InterfaceGenerator.h"

#include <DataIO/Stream/FormattedTextOutputStream.h>

#include <string>
#include <string_view>

namespace ph { class SdlField; }
namespace ph { class SdlFunction; }

namespace ph::sdlgen
{

/*! @brief Generate Python SDL interface.
Generates a Python module named `pysdl`. It is a single-file module that contains 
various utilities to help building SDL-based scene in Python.
*/
class PythonGenerator : public InterfaceGenerator
{
public:
	explicit PythonGenerator(Path outputDirectory);

	void generate(
		const std::vector<const SdlClass*>& sdlClasses,
		const std::vector<const SdlEnum*>&  sdlEnums) override;

	Path makeResourcePath(const std::string& fileSubPath) const;

private:
	FormattedTextOutputStream m_file;
};

}// end namespace ph::sdlgen
