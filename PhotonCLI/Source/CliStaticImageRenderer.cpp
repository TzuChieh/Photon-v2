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

	PhUInt32 imageWidthPx, imageHeightPx;
	phGetRenderDimension(getEngine(), &imageWidthPx, &imageHeightPx);

	std::atomic<bool> isRenderingCompleted = false;

	// REFACTOR: make a dedicated query class
	std::thread queryThread([this, imageWidthPx, imageHeightPx, &isRenderingCompleted]()
	{
		using namespace std::chrono_literals;
		using Clock = std::chrono::steady_clock;

		const auto startTime = Clock::now();

		// OPT: does not need to create this frame if intermediate frame is not requested
		PhUInt64 queryFrameId;
		phCreateFrame(&queryFrameId, imageWidthPx, imageHeightPx);

		PhFloat32 lastProgress = 0;
		PhFloat32 lastOutputProgress = 0;
		while(!isRenderingCompleted)
		{
			auto queryInterval = 2000ms;

			PhFloat32 currentProgress;
			PhFloat32 samplesPerSecond;
			phAsyncGetRenderStatistics(getEngine(), &currentProgress, &samplesPerSecond);

			if(currentProgress - lastProgress > 1.0f)
			{
				lastProgress = currentProgress;
				std::cout << "progress: " << currentProgress << " % | " 
				          << "samples/sec: " << samplesPerSecond << std::endl;
			}

			if(getArgs().isIntermediateOutputRequested())
			{
				bool shouldSaveImage = false;
				std::string imageFilePath = getArgs().getImageOutputPath() + "_intermediate_";
				if(getArgs().getIntermediateOutputIntervalUnit() == EIntervalUnit::Percentage)
				{
					if(currentProgress - lastOutputProgress > getArgs().getIntermediateOutputInterval())
					{
						shouldSaveImage = true;
						lastOutputProgress = currentProgress;

						if(!getArgs().isOverwriteRequested())
						{
							imageFilePath += std::to_string(currentProgress) + "%";
						}
					}
				}
				else if(getArgs().getIntermediateOutputIntervalUnit() == EIntervalUnit::Second)
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
					queryInterval = std::chrono::milliseconds(ims);
				}

				if(shouldSaveImage)
				{
					imageFilePath += "." + getArgs().getImageFileFormat();

					if(getArgs().isPostProcessRequested())
					{
						phAsyncPeekFrame(getEngine(), 0, 0, 0, imageWidthPx, imageHeightPx, queryFrameId);
					}
					else
					{
						phAsyncPeekFrameRaw(getEngine(), 0, 0, 0, imageWidthPx, imageHeightPx, queryFrameId);
					}

					phSaveFrame(queryFrameId, imageFilePath.c_str(), nullptr);
				}
			}

			std::this_thread::sleep_for(queryInterval);
		}// end while
	});

	renderThread.join();
	isRenderingCompleted = true;
	std::cout << "render completed" << std::endl;

	PhUInt64 frameId;
	phCreateFrame(&frameId, imageWidthPx, imageHeightPx);
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
