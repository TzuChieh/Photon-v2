#include "BlenderStaticImageRenderer.h"
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

	const bool useServer = getArgs().getPort() != 0;

	std::jthread serverThread;
	if(useServer)
	{
		serverThread = std::jthread([this, filmW, filmH](std::stop_token token)
		{
			using asio::ip::tcp;

			const std::string IP = "127.0.0.1";
			const unsigned short PORT = 7000;

			PHuint64 bufferId;
			phCreateBuffer(&bufferId);

			PHuint64 serverFrameId;
			phCreateFrame(&serverFrameId, filmW, filmH);

			try
			{
				asio::io_context io_context;

				tcp::endpoint endpoint = tcp::endpoint(asio::ip::address::from_string(IP), PORT);
				tcp::acceptor acceptor(io_context, endpoint);

				tcp::socket socket(io_context);
				acceptor.accept(socket);

				//while(!isRenderingCompleted)
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
			catch(std::exception& e)
			{
				std::cerr << e.what() << std::endl;
			}

			phDeleteBuffer(bufferId);
			phDeleteFrame(serverFrameId);
		});
	}

	queryThread.request_stop();
	renderThread.join();
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

	if(useServer)
	{
		serverThread.join();
	}

	queryThread.join();
}

}// end namespace ph::cli
