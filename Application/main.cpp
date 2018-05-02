#include "CommandLineArguments.h"
#include "util.h"

#include <ph_core.h>

#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>
#include <atomic>
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

	PHuint64 engineId;
	phCreateEngine(&engineId, static_cast<PHuint32>(args.getNumRenderThreads()));

	std::ifstream sceneFile;
	sceneFile.open(args.getSceneFilePath(), std::ios::in);
	if(!sceneFile.is_open())
	{
		std::cerr << "warning: scene file <" << args.getSceneFilePath() << "> opening failed" << std::endl;
		return EXIT_FAILURE;
	}
	else
	{
		std::string lineCommand;
		while(sceneFile.good())
		{
			std::getline(sceneFile, lineCommand);
			lineCommand += '\n';
			phEnterCommand(engineId, lineCommand.c_str());
		}
		phEnterCommand(engineId, "->");

		sceneFile.close();
	}

	phUpdate(engineId);

	std::thread renderThread([=]()
	{
		phRender(engineId);
	});

	std::atomic<bool> isRenderingCompleted = false;
	std::thread queryThread([&]()
	{
		/*PHuint32 x, y, w, h;
		int regionStatus = phAsyncPollUpdatedFilmRegion(engineId, &x, &y, &w, &h);
		if(regionStatus != PH_FILM_REGION_STATUS_INVALID)
		{
			std::cout << "xywh: " << x << ", " << y << ", " << w << ", " << h << std::endl;
		}*/

		using namespace std::chrono_literals;

		PHfloat32 currentProgress = 0, samplesPerSecond = 0;
		PHfloat32 lastProgress = 0;
		while(!isRenderingCompleted)
		{
			phAsyncGetRendererStatistics(engineId, &currentProgress, &samplesPerSecond);

			if((currentProgress - lastProgress) > 1.0f)
			{
				lastProgress = currentProgress;
				std::cout << "progress: " << currentProgress << " % | " 
				          << "samples/sec: " << samplesPerSecond << std::endl;
			}

			std::this_thread::sleep_for(2s);
		}
	});

	renderThread.join();
	isRenderingCompleted = true;

	PHuint32 filmWpx, filmHpx;
	phGetFilmDimension(engineId, &filmWpx, &filmHpx);

	PHuint64 frameId;
	phCreateFrame(&frameId, filmWpx, filmHpx);
	if(args.performPostPorcess())
	{
		phDevelopFilm(engineId, frameId);
	}
	else
	{
		phDevelopFilmRaw(engineId, frameId);
	}
	phSaveFrame(frameId, args.getImageFilePath().c_str());

	std::cout << "render completed, image saved to <" << args.getImageFilePath() << ">" << std::endl;

	if(!phExit())
	{
		std::cerr << "Photon exiting failed" << std::endl;
		return EXIT_FAILURE;
	}

	queryThread.join();

	return EXIT_SUCCESS;
}