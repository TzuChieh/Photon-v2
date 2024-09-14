#include "ProcessedArguments.h"
#include "util.h"
#include "CliStaticImageRenderer.h"
#include "BlenderStaticImageRenderer.h"

#include <ph_c_core.h>
#include <Common/stats.h>

#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <algorithm>
#include <iomanip>
#include <cmath>
#include <filesystem>

using namespace ph::cli;

namespace ph::cli
{

void renderImageSeries(const ProcessedArguments& args);

}

int main(int argc, char* argv[])
{
	if(argc <= 1)
	{
		std::cout << "Photon Renderer\n";
		std::cout << "Use --help for a list of available commands.\n";
		return EXIT_SUCCESS;
	}

	if(!phInit())
	{
		std::cerr << "Photon initializing failed\n";
		return EXIT_FAILURE;
	}

	ProcessedArguments args(argc, argv);
	if(args.getExecutionMode() == EExecutionMode::Help)
	{
		args.printHelpMessage();
		return EXIT_SUCCESS;
	}

	// Begin engine operations

	// HACK
	if(args.isFrameDiagRequested())
	{
		std::cout << "begin frame diag\n";

		PhUInt64 frameA, frameB, resultFrame;
		phCreateFrame(&frameA, 0, 0);
		phCreateFrame(&frameB, 0, 0);
		phCreateFrame(&resultFrame, 0, 0);

		phLoadFrame(frameA, args.getFramePathA().c_str());
		phLoadFrame(frameB, args.getFramePathB().c_str());

		phFrameOpAbsDifference(frameA, frameB, resultFrame);
		phSaveFrame(resultFrame, "./frame_diag_abs_diff.exr", nullptr);

		const float MSE = phFrameOpMSE(frameA, frameB);
		{
			std::ostream transientCout(std::cout.rdbuf());
			transientCout << std::setprecision(20) << "MSE  = " << MSE << '\n';
			transientCout << std::setprecision(20) << "RMSE = " << std::sqrt(MSE) << '\n';
		}

		std::cout << "end frame diag" << '\n';
	}

	if(args.getExecutionMode() == EExecutionMode::SingleImage)
	{
		CliStaticImageRenderer renderer(args);
		renderer.render();
	}
	else if(args.getExecutionMode() == EExecutionMode::ImageSeries)
	{
		renderImageSeries(args);
	}
	else if(args.getExecutionMode() == EExecutionMode::Blender)
	{
		BlenderStaticImageRenderer renderer(args);
		renderer.render();
	}
	else
	{
		std::cerr << "Unknown execution mode <" << static_cast<int>(args.getExecutionMode()) << ">.\n";
		ProcessedArguments::printHelpMessage();
	}

	std::cout << ph::TimerStatsReport().detailedReport() << '\n';

	// End engine operations

	if(!phExit())
	{
		std::cerr << "Photon exiting failed\n";
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

namespace ph::cli
{

void renderImageSeries(const ProcessedArguments& args)
{
	namespace fs = std::filesystem;

	const fs::path    sceneDirectory    = fs::path(args.getSceneFilePath()).parent_path();
	const std::string sceneFilenameStar = fs::path(args.getSceneFilePath()).filename().string();
	const std::string sceneFilenameBase = sceneFilenameStar.substr(0, sceneFilenameStar.find('*'));

	// REFACTOR: getReference rid of pair, name the parameters
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

		const std::size_t wildcardedStringSize = filename.size() - filenameBase.size() - 3;
		const std::string wildcardedString     = filename.substr(filenameBase.size(), wildcardedStringSize);

		sceneFiles.push_back({wildcardedString, path.string()});
	}
	std::sort(sceneFiles.begin(), sceneFiles.end());

	fs::create_directories(args.getImageOutputPath());

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
		CliStaticImageRenderer renderer(args);
		renderer.setSceneFilePath(sceneFile.second);

		const std::string imageFilename = sceneFile.first;
		const fs::path    imageFilePath = fs::path(args.getImageOutputPath()) / imageFilename;
		renderer.setImageOutputPath(imageFilePath.string());
		
		renderer.render();
	}
}

}// end namespace ph::cli
