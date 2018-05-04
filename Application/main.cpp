#include "CommandLineArguments.h"
#include "util.h"
#include "StaticImageRenderer.h"

#include <ph_core.h>

#include <iostream>
#include <string>
#include <vector>
#include <experimental/filesystem>
#include <utility>
#include <algorithm>

using namespace PH_CLI_NAMESPACE;

void renderImageSeries(const CommandLineArguments& args);

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
	if(args.isHelpMessageRequested())
	{
		CommandLineArguments::printHelpMessage();
		return EXIT_SUCCESS;
	}

	if(!phInit())
	{
		std::cerr << "Photon initialing failed" << std::endl;
		return EXIT_FAILURE;
	}

	if(!args.isImageSeriesRequested())
	{
		StaticImageRenderer renderer(args);
		renderer.render();
	}
	else
	{
		renderImageSeries(args);
	}

	if(!phExit())
	{
		std::cerr << "Photon exiting failed" << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

void renderImageSeries(const CommandLineArguments& args)
{
	namespace fs = std::experimental::filesystem;

	const fs::path    sceneDirectory    = fs::path(args.getSceneFilePath()).parent_path();
	const std::string sceneFilenameStar = fs::path(args.getSceneFilePath()).filename().string();
	const std::string sceneFilenameBase = sceneFilenameStar.substr(0, sceneFilenameStar.find('*'));

	typedef std::pair<std::string, std::string> StringPair;
	std::vector<StringPair> sceneFiles;
	for(const auto& directory : fs::directory_iterator(sceneDirectory))
	{
		const fs::path path = directory.path();
		if(path.extension() != ".p2")
		{
			continue;
		}

		const std::string filename     = path.filename().string();
		const std::string filenameBase = filename.substr(0, sceneFilenameBase.size());
		if(filenameBase != sceneFilenameBase)
		{
			continue;
		}

		const std::size_t stringSize = filename.size() - filenameBase.size() - 3;
		const std::string string     = filename.substr(filenameBase.size(), stringSize);

		sceneFiles.push_back({string, path.string()});
	}
	std::sort(sceneFiles.begin(), sceneFiles.end());

	fs::create_directories(args.getImageFilePath());

	auto sceneBegin = std::find_if(sceneFiles.begin(), sceneFiles.end(), 
		[&args](const std::pair<std::string, std::string>& sceneFile)
		{
			return sceneFile.first == args.wildcardStart();
		});
	sceneBegin = sceneBegin != sceneFiles.end() ? sceneBegin : sceneFiles.begin();

	auto sceneEnd = std::find_if(sceneBegin, sceneFiles.end(),
		[&args](const std::pair<std::string, std::string>& sceneFile)
		{
			return sceneFile.first == args.wildcardFinish();
		});
	sceneEnd = sceneEnd != sceneFiles.end() ? sceneEnd + 1 : sceneEnd;

	sceneFiles = std::vector<StringPair>(sceneBegin, sceneEnd);
	if(sceneFiles.empty())
	{
		std::cout << "no file matching <" << args.getSceneFilePath() << "> found" << std::endl;
		std::cout << "(within wildcard range [" << args.wildcardStart() << ", " << args.wildcardFinish() << "])" << std::endl;
		return;
	}

	for(const auto& sceneFile : sceneFiles)
	{
		StaticImageRenderer renderer(args);
		renderer.setSceneFilePath(sceneFile.second);

		const std::string imageFilename = sceneFile.first + ".png";
		const fs::path    imageFilePath = fs::path(args.getImageFilePath()) / imageFilename;
		renderer.setImageFilePath(imageFilePath.string());
		
		renderer.render();
	}
}