#include "BlenderStaticImageRenderer.h"
#include "util.h"

#include <Common/logging.h>

#include <asio.hpp>

#include <iostream>
#include <string>
#include <thread>
#include <chrono>

namespace ph::cli
{

PH_DEFINE_INTERNAL_LOG_GROUP(Blender, PhotonCLI);

BlenderStaticImageRenderer::BlenderStaticImageRenderer(const ProcessedArguments& args)
	: StaticImageRenderer(args)
{}

void BlenderStaticImageRenderer::render()
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

	std::jthread queryThread([this](std::stop_token token)
	{
		using namespace std::chrono_literals;
		using Clock = std::chrono::steady_clock;

		const auto startTime = Clock::now();

		PHfloat32 lastProgress = 0;
		PHfloat32 lastOutputProgress = 0;
		while(!token.stop_requested())
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

			std::this_thread::sleep_for(2s);
		}
	});

	const unsigned short port = getArgs().getPort();

	std::jthread serverThread;
	serverThread = std::jthread([this, filmW, filmH, port](std::stop_token token)
	{
		PHuint64 bufferId;
		phCreateBuffer(&bufferId);

		PHuint64 serverFrameId;
		phCreateFrame(&serverFrameId, filmW, filmH);

		try
		{
			asio::io_context ioContext;

			// Server endpoint listening to the specified port
			asio::ip::tcp::endpoint endpoint(asio::ip::tcp::v4(), port);
			asio::ip::tcp::acceptor acceptor(ioContext, endpoint);
			PH_LOG(Blender, "Server listening on port {}", port);

			// A blocking accept, unblocks only if a connection is accepted or an error occurs
			asio::ip::tcp::socket socket(ioContext);
			acceptor.accept(socket);
			PH_LOG(Blender, "Connection accepted.");

			while(!token.stop_requested())
			{
				phAsyncPeekFrameRaw(getEngine(), 0, 0, 0, filmW, filmH, serverFrameId);
				phSaveFrameToBuffer(serverFrameId, bufferId);

				const unsigned char* bytesPtr;
				size_t numBytes;
				phGetBufferBytes(bufferId, &bytesPtr, &numBytes);

				std::cerr << "SERVER: sending..." << std::endl;

				asio::error_code ignored_error;
				const auto numBytes64 = static_cast<std::uint64_t>(numBytes);
				asio::write(socket, asio::buffer(reinterpret_cast<const unsigned char*>(&numBytes64), 8), ignored_error);
				asio::write(socket, asio::buffer(bytesPtr, numBytes), ignored_error);

				std::cerr << "SERVER: sent" << std::endl;
			}
		}
		catch(const std::exception& e)
		{
			PH_LOG_ERROR(Blender, "Error on establishing connection: {}.", e.what());
		}

		phDeleteBuffer(bufferId);
		phDeleteFrame(serverFrameId);
	});

	if(renderThread.joinable())
	{
		renderThread.join();
	}
	PH_LOG(Blender, "Render finished.");

	queryThread.request_stop();
	serverThread.request_stop();

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
}

}// end namespace ph::cli
