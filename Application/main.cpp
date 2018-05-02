#include "CommandLineArguments.h"
#include "util.h"
#include "StaticImageRenderer.h"

#include <ph_core.h>

#include <iostream>
#include <string>
#include <vector>

using namespace PH_CLI_NAMESPACE;

int main(int argc, char* argv[])
{
	if(argc <= 1)
	{
		std::cout << "Photon-v2 Renderer" << std::endl;
		std::cout << "Use --help for a list of available commands." << std::endl;
		return EXIT_SUCCESS;
	}

	std::vector<std::string> arguments;
	for(int i = 0; i < argc; i++)
	{
		arguments.push_back(argv[i]);
	}

	CommandLineArguments args(arguments);
	if(args.helpMessageRequested())
	{
		CommandLineArguments::printHelpMessage();
		return EXIT_SUCCESS;
	}

	if(!phInit())
	{
		std::cerr << "Photon initialing failed" << std::endl;
		return EXIT_FAILURE;
	}

	StaticImageRenderer renderer(args);
	renderer.render();

	if(!phExit())
	{
		std::cerr << "Photon exiting failed" << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}