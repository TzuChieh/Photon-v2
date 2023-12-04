#include "CliStaticImageRenderer.h"
#include "util.h"

#include <asio.hpp>

#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <atomic>
#include <limits>
#include <cstdint>

// FIXME: add osx fs headers once it is supported
#if defined(_WIN32)
	#include <filesystem>
#elif defined(__linux__)
	#include <experimental/filesystem>
#endif

namespace ph::cli
{

CliStaticImageRenderer::CliStaticImageRenderer(const ProcessedArguments& args)
	: StaticImageRenderer(args)
{}

void CliStaticImageRenderer::render()
{
	setSceneFilePath(getArgs().getSceneFilePath());

	if(!loadCommandsFromSceneFile())
	{
		return;
	}

	phUpdate(getEngine());

	std::thread renderThread([this]()
	{
		phRender(getEngine());
	});

	PHuint32 filmW, filmH;
	phGetRenderDimension(getEngine(), &filmW, &filmH);

	std::atomic<bool> isRenderingCompleted = false;

	// REFACTOR: make a dedicated query class
	std::thread queryThread([this, filmW, filmH, &isRenderingCompleted]()
	{
		using namespace std::chrono_literals;
		using Clock = std::chrono::steady_clock;

		const auto startTime = Clock::now();

		// OPT: does not need to create this frame if intermediate frame is not requested
		PHuint64 queryFrameId;
		phCreateFrame(&queryFrameId, filmW, filmH);

		PHfloat32 lastProgress = 0;
		PHfloat32 lastOutputProgress = 0;
		while(!isRenderingCompleted)
		{
			PHfloat32 currentProgress;
			PHfloat32 samplesPerSecond;
			phAsyncGetRendererStatistics(getEngine(), &currentProgress, &samplesPerSecond);

			if(currentProgress - lastProgress > 1.0f)
			{
				lastProgress = currentProgress;
				std::cout << "progress: " << currentProgress << " % | " 
				          << "samples/sec: " << samplesPerSecond << std::endl;
			}

			bool shouldSaveImage = false;
			std::string imageFilePath = getArgs().getImageOutputPath() + "_intermediate_";
			if(getArgs().getIntervalUnit() == EIntervalUnit::Percentage)
			{
				if(currentProgress - lastOutputProgress > getArgs().getIntermediateOutputInterval())
				{
					shouldSaveImage = true;
					lastOutputProgress = currentProgress;

					if(!getArgs().isOverwriteRequested())
					{
						imageFilePath += std::to_string(currentProgress) + "%";
					}

					std::this_thread::sleep_for(2s);
				}
			}
			else if(getArgs().getIntervalUnit() == EIntervalUnit::Second)
			{
				const auto currentTime = Clock::now();
				const auto duration = currentTime - startTime;
				const auto deltaMs = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();

				shouldSaveImage = true;

				if(!getArgs().isOverwriteRequested())
				{
					imageFilePath += std::to_string(deltaMs / 1000.0f) + "s";
				}

				const float fms = getArgs().getIntermediateOutputInterval() * 1000;
				const int ims = fms < std::numeric_limits<int>::max() ? static_cast<int>(fms) : std::numeric_limits<int>::max();
				std::this_thread::sleep_for(std::chrono::milliseconds(ims));
			}

			if(shouldSaveImage)
			{
				imageFilePath += "." + getArgs().getImageFileFormat();

				if(getArgs().isPostProcessRequested())
				{
					phAsyncPeekFrame(getEngine(), 0, 0, 0, filmW, filmH, queryFrameId);
				}
				else
				{
					phAsyncPeekFrameRaw(getEngine(), 0, 0, 0, filmW, filmH, queryFrameId);
				}

				phSaveFrame(
					queryFrameId,
					imageFilePath.c_str());
			}
		}// end while
	});

	renderThread.join();
	isRenderingCompleted = true;
	std::cout << "render completed" << std::endl;

	PHuint64 frameId;
	phCreateFrame(&frameId, filmW, filmH);
	if(getArgs().isPostProcessRequested())
	{
		phAquireFrame(getEngine(), 0, frameId);
	}
	else
	{
		phAquireFrameRaw(getEngine(), 0, frameId);
	}

	save_frame_with_fail_safe(frameId, getArgs().getImageFilePath());
	phDeleteFrame(frameId);

	queryThread.join();
}

}// end namespace ph::cli
