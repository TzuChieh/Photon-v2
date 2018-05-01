#include <ph_core.h>

#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>
#include <atomic>

void printHelpMessage();

int main(int argc, char* argv[])
{
	if(argc <= 1)
	{
		std::cout << "Photon-v2 Renderer" << std::endl;
		std::cout << "Use --help for a list of available commands." << std::endl;
		return EXIT_SUCCESS;
	}

	int numRenderThreads = 1;
	std::string sceneFilePath = "./";
	std::string imageFilePath = "./result.png";
	bool postProcessFrame = true;
	for(int i = 1; i < argc; i++)
	{
		if(std::string(argv[i]) == "-s")
		{
			i++;
			if(i < argc)
			{
				sceneFilePath = argv[i];
			}
		}
		else if(std::string(argv[i]) == "-i")
		{
			i++;
			if(i < argc)
			{
				imageFilePath = argv[i];
			}
		}
		else if(std::string(argv[i]) == "-t")
		{
			i++;
			if(i < argc)
			{
				numRenderThreads = std::stoi(argv[i]);
				if(numRenderThreads <= 0)
				{
					std::cerr << "warning: bad number of threads <" << argv[i] << std::endl;
					std::cerr << "use 1 instead" << std::endl;
				}
			}
		}
		else if(std::string(argv[i]) == "--raw")
		{
			postProcessFrame = false;
		}
		else if(std::string(argv[i]) == "--help")
		{
			printHelpMessage();
			return EXIT_SUCCESS;
		}
		else
		{
			std::cerr << "warning: unknown command <" << argv[i] << ">" << std::endl;
			std::cerr << "ignored" << std::endl;
		}
	}

	if(!phInit())
	{
		std::cerr << "Photon initialing failed" << std::endl;
		return EXIT_FAILURE;
	}

	PHuint64 engineId;
	phCreateEngine(&engineId, static_cast<PHuint32>(numRenderThreads));

	std::ifstream sceneFile;
	sceneFile.open(sceneFilePath, std::ios::in);
	if(!sceneFile.is_open())
	{
		std::cerr << "warning: scene file <" << sceneFilePath << "> opening failed" << std::endl;
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
	if(postProcessFrame)
	{
		phDevelopFilm(engineId, frameId);
	}
	else
	{
		phDevelopFilmRaw(engineId, frameId);
	}
	phSaveFrame(frameId, imageFilePath.c_str());

	std::cout << "render completed, image saved to <" << imageFilePath << ">" << std::endl;

	if(!phExit())
	{
		std::cerr << "Photon exiting failed" << std::endl;
		return EXIT_FAILURE;
	}

	queryThread.join();

	return EXIT_SUCCESS;
}

void printHelpMessage()
{
	std::cout << R"(

	-s <path>      specify the scene file to render
	-i <path>      specify the output image
	-t <number>    number of thread for rendering
	--raw          do not perform any post-processing
	--help         print this help message then exit

	)" << std::endl;
}