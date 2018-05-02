#include "StaticImageRenderer.h"
#include "CommandLineArguments.h"

#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>
#include <atomic>

PH_CLI_NAMESPACE_BEGIN

StaticImageRenderer::StaticImageRenderer(const CommandLineArguments& args) :
	m_engineId(0),
	m_sceneFilePath(args.getSceneFilePath()),
	m_imageFilePath(args.getImageFilePath()),
	m_numRenderThreads(args.getNumRenderThreads()),
	m_performPostProcessing(args.performPostPorcess())
{
	phCreateEngine(&m_engineId, static_cast<PHuint32>(m_numRenderThreads));
}

void StaticImageRenderer::render() const
{
	if(!loadCommandsFromSceneFile())
	{
		return;
	}

	phUpdate(m_engineId);

	std::thread renderThread([=]()
	{
		phRender(m_engineId);
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
			phAsyncGetRendererStatistics(m_engineId, &currentProgress, &samplesPerSecond);

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
	phGetFilmDimension(m_engineId, &filmWpx, &filmHpx);

	PHuint64 frameId;
	phCreateFrame(&frameId, filmWpx, filmHpx);
	if(m_performPostProcessing)
	{
		phDevelopFilm(m_engineId, frameId);
	}
	else
	{
		phDevelopFilmRaw(m_engineId, frameId);
	}
	phSaveFrame(frameId, m_imageFilePath.c_str());

	std::cout << "render completed, image saved to <" << m_imageFilePath << ">" << std::endl;
}

bool StaticImageRenderer::loadCommandsFromSceneFile() const
{
	std::ifstream sceneFile;
	sceneFile.open(m_sceneFilePath, std::ios::in);
	if(!sceneFile.is_open())
	{
		std::cerr << "warning: scene file <" << m_sceneFilePath << "> opening failed" << std::endl;
		return false;
	}
	else
	{
		std::string lineCommand;
		while(sceneFile.good())
		{
			std::getline(sceneFile, lineCommand);
			lineCommand += '\n';
			phEnterCommand(m_engineId, lineCommand.c_str());
		}
		phEnterCommand(m_engineId, "->");
	}
}

PH_CLI_NAMESPACE_END