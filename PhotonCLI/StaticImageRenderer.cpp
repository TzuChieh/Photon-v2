#include "StaticImageRenderer.h"
#include "CommandLineArguments.h"

#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>
#include <atomic>

// FIXME: add osx fs headers once it is supported
#if defined(_WIN32)
	#include <filesystem>
#elif defined(__linux__)
	#include <experimental/filesystem>
#endif

PH_CLI_NAMESPACE_BEGIN

StaticImageRenderer::StaticImageRenderer(const CommandLineArguments& args) :
	m_engineId(0),
	m_sceneFilePath(),
	m_imageFilePath(args.getImageFilePath()),
	m_numRenderThreads(args.getNumRenderThreads()),
	m_isPostProcessRequested(args.isPostProcessRequested()),
	m_outputPercentageProgress(args.getOutputPercentageProgress())
{
	phCreateEngine(&m_engineId, static_cast<PHuint32>(m_numRenderThreads));

	setSceneFilePath(args.getSceneFilePath());
}

StaticImageRenderer::~StaticImageRenderer()
{
	phDeleteEngine(m_engineId);
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

	PHuint32 filmWpx, filmHpx;
	phGetRenderDimension(m_engineId, &filmWpx, &filmHpx);

	std::atomic<bool> isRenderingCompleted = false;
	std::thread queryThread([&]()
	{
		using namespace std::chrono_literals;

		// OPT: does not need to create this frame if intermediate frame is not requested
		PHuint64 queryFrameId;
		phCreateFrame(&queryFrameId, filmWpx, filmHpx);

		PHfloat32 lastProgress = 0;
		PHfloat32 lastOutputProgress = 0;
		while(!isRenderingCompleted)
		{
			PHfloat32 currentProgress;
			PHfloat32 samplesPerSecond;
			phAsyncGetRendererStatistics(m_engineId, &currentProgress, &samplesPerSecond);

			if(currentProgress - lastProgress > 1.0f)
			{
				lastProgress = currentProgress;
				std::cout << "progress: " << currentProgress << " % | " 
				          << "samples/sec: " << samplesPerSecond << std::endl;
			}

			if(currentProgress - lastOutputProgress > m_outputPercentageProgress)
			{
				PHuint32 qx, qy, qw, qh;
				int regionStatus = phAsyncPollUpdatedFrameRegion(m_engineId, &qx, &qy, &qw, &qh);
				if(regionStatus != PH_FILM_REGION_STATUS_INVALID)
				{
					phAsyncPeekFrame(m_engineId, 0, qx, qy, qw, qh, queryFrameId);

					phSaveFrame(
						queryFrameId, 
						(m_imageFilePath + "_" + std::to_string(currentProgress) + "%.png").c_str());
				}

				lastOutputProgress = currentProgress;
			}

			std::this_thread::sleep_for(2s);
		}
	});

	renderThread.join();
	isRenderingCompleted = true;
	std::cout << "render completed" << std::endl;

	PHuint64 frameId;
	phCreateFrame(&frameId, filmWpx, filmHpx);
	if(m_isPostProcessRequested)
	{
		phAquireFrame(m_engineId, 0, frameId);
	}
	else
	{
		phAquireFrameRaw(m_engineId, 0, frameId);
	}

	save_frame_with_fail_safe(frameId, m_imageFilePath);

	phDeleteFrame(frameId);

	queryThread.join();
}

void StaticImageRenderer::setSceneFilePath(const std::string& path)
{
	m_sceneFilePath = path;

#ifndef __APPLE__
	namespace fs = std::experimental::filesystem;
	const std::string sceneDirectory = fs::path(path).parent_path().string();
	phSetWorkingDirectory(m_engineId, sceneDirectory.c_str());
#else
	const std::size_t slashIndex = path.find_last_of('/');
	if(slashIndex != std::string::npos)
	{
		const std::string sceneDirectory = path.substr(0, slashIndex + 1);
		phSetWorkingDirectory(m_engineId, sceneDirectory.c_str());
	}
	else
	{
		std::cerr << "warning: cannot retrieve scene directory from path <" << path << ">" << std::endl;
	}
#endif
}

void StaticImageRenderer::setImageFilePath(const std::string& path)
{
	m_imageFilePath = path;
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
		std::cerr << "loading scene file <" << m_sceneFilePath << ">" << std::endl;

		std::string lineCommand;
		while(sceneFile.good())
		{
			std::getline(sceneFile, lineCommand);
			lineCommand += '\n';
			phEnterCommand(m_engineId, lineCommand.c_str());
		}
		phEnterCommand(m_engineId, "->");

		return true;
	}
}

PH_CLI_NAMESPACE_END