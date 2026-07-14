#include "CliStaticImageRenderer.h"
#include "util.h"

#include <asio.hpp>

#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <atomic>
#include <cstdint>
#include <algorithm>

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

	phUpdate(getSession());

	std::thread renderThread([this]()
	{
		phRender(getSession());
	});

	PhUInt32 imageWidthPx, imageHeightPx;
	phGetRenderDimension(getSession(), &imageWidthPx, &imageHeightPx);

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
			phAsyncGetRenderStatistics(getSession(), &currentProgress, &samplesPerSecond);

			if(currentProgress - lastProgress > 1.0f)
			{
				lastProgress = currentProgress;
				std::cout << "progress: " << currentProgress << " % | " 
				          << "samples/sec: " << samplesPerSecond << std::endl;
			}

			if(getArgs().isIntermediateOutputRequested())
			{
				bool shouldSaveImage = false;
				std::string imageFilePath = getArgs().getPrimaryImageOutputStem() + "_intermediate_";
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

					const float32 fms = std::clamp(getArgs().getIntermediateOutputInterval() * 1000, 0.0f, 86400.0f);
					queryInterval = std::chrono::milliseconds(static_cast<int>(fms));
				}

				if(shouldSaveImage)
				{
					imageFilePath += "." + getArgs().getImageFileFormat();

					if(getArgs().isPostProcessRequested())
					{
						phAsyncPeekFrame(getSession(), 0, 0, 0, imageWidthPx, imageHeightPx, queryFrameId);
					}
					else
					{
						phAsyncPeekFrameRaw(getSession(), 0, 0, 0, imageWidthPx, imageHeightPx, queryFrameId);
					}

					phSaveFrame(queryFrameId, imageFilePath.c_str(), nullptr);
				}
			}

			std::this_thread::sleep_for(queryInterval);
		}// end while

		phDeleteFrame(queryFrameId);
	});

	renderThread.join();
	isRenderingCompleted = true;
	
	std::cout << "render completed" << std::endl;

	PhUInt32 numOutputLayers = 1;
	PhRenderObservableInfo observableInfo{};
	phGetRenderObservableInfo(getSession(), &observableInfo);
	if(observableInfo.numLayers > 0)
	{
		numOutputLayers = observableInfo.numLayers;
	}

	PhUInt64 frameId;
	phCreateFrame(&frameId, imageWidthPx, imageHeightPx);
	for(PhUInt32 layerIndex = 0; layerIndex < numOutputLayers; ++layerIndex)
	{
		if(getArgs().isPostProcessRequested())
		{
			phRetrieveFrame(getSession(), layerIndex, frameId);
		}
		else
		{
			phRetrieveFrameRaw(getSession(), layerIndex, frameId);
		}

		std::string layerName;
		PhSize layerNameLength = 0;
		phGetRenderLayerName(getSession(), layerIndex, nullptr, &layerNameLength);
		layerName.resize(layerNameLength - 1);
		phGetRenderLayerName(getSession(), layerIndex, layerName.data(), nullptr);

		save_frame_with_fail_safe(
			frameId,
			getArgs().getImageFilePath(layerIndex, numOutputLayers),
			nullptr,
			layerName);
	}
	phDeleteFrame(frameId);

	queryThread.join();
}

}// end namespace ph::cli
