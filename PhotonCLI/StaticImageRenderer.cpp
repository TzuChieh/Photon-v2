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

StaticImageRenderer::StaticImageRenderer(const ProcessedArguments& args) :
	m_engineId(0),
	m_args(args)
{
	phCreateEngine(&m_engineId, static_cast<PHuint32>(args.getNumRenderThreads()));
}

StaticImageRenderer::~StaticImageRenderer()
{
	phDeleteEngine(m_engineId);
}

void StaticImageRenderer::render()
{
	setSceneFilePath(m_args.getSceneFilePath());

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

			if(currentProgress - lastOutputProgress > m_args.getOutputPercentageProgress())
			{
				PHuint32 qx, qy, qw, qh;
				int regionStatus = phAsyncPollUpdatedFrameRegion(m_engineId, &qx, &qy, &qw, &qh);
				if(regionStatus != PH_FILM_REGION_STATUS_INVALID)
				{
					std::cerr << "x=" << qx << ", y=" << qy << ", qw=" << qw << ", qh=" << qh << std::endl;
					phAsyncPeekFrame(m_engineId, 0, qx, qy, qw, qh, queryFrameId);

					const auto intermediateImageFilePath = 
						m_args.getImageOutputPath() + 
						"_intermediate_" + std::to_string(currentProgress) + "%" + 
						"." + m_args.getImageFileFormat();
					phSaveFrame(
						queryFrameId, 
						intermediateImageFilePath.c_str());
				}

				/*phAsyncPeekFrame(m_engineId, 0, 0, 0, filmWpx, filmHpx, queryFrameId);

				const auto intermediateImageFilePath =
					m_args.getImageOutputPath() +
					"_intermediate_" + std::to_string(currentProgress) + "%" +
					"." + m_args.getImageFileFormat();
				phSaveFrame(
					queryFrameId,
					intermediateImageFilePath.c_str());*/

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
	if(m_args.isPostProcessRequested())
	{
		phAquireFrame(m_engineId, 0, frameId);
	}
	else
	{
		phAquireFrameRaw(m_engineId, 0, frameId);
	}

	save_frame_with_fail_safe(frameId, m_args.getImageFilePath());

	phDeleteFrame(frameId);

	queryThread.join();
}

void StaticImageRenderer::setSceneFilePath(const std::string& path)
{
	m_args.setSceneFilePath(path);

// REFACTOR: use a get directory-from-file-path function
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

void StaticImageRenderer::setImageOutputPath(const std::string& path)
{
	m_args.setImageOutputPath(path);
}

bool StaticImageRenderer::loadCommandsFromSceneFile() const
{
	const auto sceneFilePath = m_args.getSceneFilePath();

	std::ifstream sceneFile;
	sceneFile.open(sceneFilePath, std::ios::in);
	if(!sceneFile.is_open())
	{
		std::cerr << "warning: scene file <" << sceneFilePath << "> opening failed" << std::endl;
		return false;
	}
	else
	{
		std::cerr << "loading scene file <" << sceneFilePath << ">" << std::endl;

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