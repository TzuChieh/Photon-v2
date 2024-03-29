#include "SdlGenArguments.h"

#include <Common/logging.h>

#include <string_view>

namespace ph::sdlgen
{

PH_DEFINE_INTERNAL_LOG_GROUP(SdlGenCliArgs, SDLGenCLI);

SdlGenArguments::SdlGenArguments(int argc, char* argv[]) :
	SdlGenArguments(CommandLineArguments(argc, argv))
{}

SdlGenArguments::SdlGenArguments(CommandLineArguments arguments) :
	m_executionMode         (ESdlGenMode::Unspecified),
	m_interfaceGeneratorType(EInterfaceGenerator::Unspecified),
	m_outputPath            ("./SDLGenCLI_output/")
{
	while(!arguments.isEmpty())
	{
		const std::string argument = arguments.retrieveString();

		if(argument == "--interface")
		{
			m_executionMode = ESdlGenMode::INTERFACE_GENERATION;

			const std::string interfaceType = arguments.retrieveString();
			if(interfaceType == "markdown-doc")
			{
				m_interfaceGeneratorType = EInterfaceGenerator::MARKDOWN_DOC;
			}
			else if(interfaceType == "python")
			{
				m_interfaceGeneratorType = EInterfaceGenerator::PYTHON;
			}
			else
			{
				PH_LOG(SdlGenCliArgs, Warning, "unknown interface generation type specified: {}",
					interfaceType);

				m_interfaceGeneratorType = EInterfaceGenerator::Unspecified;
			}
		}
		else if(argument == "--output" || argument == "-o")
		{
			m_outputPath = Path(arguments.retrieveString());
		}
		else if(argument == "--help" || argument == "-h")
		{
			m_executionMode = ESdlGenMode::PRINT_HELP_MESSAGE;
		}
		else
		{
			PH_LOG(SdlGenCliArgs, Warning, "unknown command <{}> specified, ignoring",
				argument);
		}
	}// end while more arguments exist

	// TODO: argument sanity check
}

}// end namespace ph::sdlgen
