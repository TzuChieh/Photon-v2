#include "SDLGenCLI/SdlGenArguments.h"

#include <ph_core.h>
#include <SDLGen/ph_sdlgen.h>
#include <Common/logging.h>

#include <iostream>
#include <cstdlib>

int main(int argc, char* argv[])
{
	using namespace ph;
	using namespace ph::sdlgen;

	if(!init_render_engine())
	{
		std::cerr << "Photon Renderer initialization failed" << std::endl;
		return EXIT_FAILURE;
	}

	SdlGenArguments arguments(argc, argv);
	if(arguments.getExecutionMode() == ESdlGenMode::Unspecified)
	{
		std::cout << "Photon-v2 SDL Generation" << std::endl;
		std::cout << "Use --help or -h for how to use the program." << std::endl;
		return EXIT_SUCCESS;
	}

	if(arguments.getExecutionMode() == ESdlGenMode::PrintHelpMessage)
	{
		std::cout << SdlGenArguments::genHelpMessage() << std::endl;
		return EXIT_SUCCESS;
	}

	if(arguments.getExecutionMode() == ESdlGenMode::InterfaceGeneration)
	{
		generate_sdl_interface(
			arguments.getInterfaceGeneratorType(), 
			arguments.getOutputPath().toAbsoluteString());

		return EXIT_SUCCESS;
	}

	if(!exit_render_engine())
	{
		std::cerr << "Photon Renderer exiting failed" << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
