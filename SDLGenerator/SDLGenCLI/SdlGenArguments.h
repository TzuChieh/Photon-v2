#pragma once

#include <Utility/CommandLineArguments.h>
#include <Common/primitive_type.h>
#include <DataIO/FileSystem/Path.h>
#include <ph_sdlgen.h>

#include <string>

namespace ph::sdlgen
{

enum class ESdlGenMode
{
	UNSPECIFIED,
	PRINT_HELP_MESSAGE,
	INTERFACE_GENERATION
};

class SdlGenArguments final
{
public:
	static std::string genHelpMessage();

public:
	SdlGenArguments(int argc, char* argv[]);
	explicit SdlGenArguments(CommandLineArguments arguments);

	ESdlGenMode getExecutionMode() const;
	EInterfaceGenerator getInterfaceGeneratorType() const;
	const Path& getOutputPath() const;

private:
	ESdlGenMode         m_executionMode;
	EInterfaceGenerator m_interfaceGeneratorType;
	Path                m_outputPath;
};

// In-header Implementations:

inline ESdlGenMode SdlGenArguments::getExecutionMode() const
{
	return m_executionMode;
}

inline EInterfaceGenerator SdlGenArguments::getInterfaceGeneratorType() const
{
	return m_interfaceGeneratorType;
}

inline const Path& SdlGenArguments::getOutputPath() const
{
	return m_outputPath;
}

inline std::string SdlGenArguments::genHelpMessage()
{
	return R"(
===============================================================================
--interface <type>
 
Tell the program to generate SDL interface. Possible types are:
  markdown-doc: documentation for SDL in markdown format
  python: Python interface for producing SDL
===============================================================================
{--output | -o} <path>

Specify a path that will store the generated output. This can either be a 
filename or a directory depending on the task.
(default to: "./SDLGenCLI_output/")
===============================================================================
{--help | -h}

Print this help message then exit.
===============================================================================
	)";
}

}// end namespace ph::sdlgen
