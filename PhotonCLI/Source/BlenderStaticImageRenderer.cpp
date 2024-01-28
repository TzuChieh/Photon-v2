#include "BlenderStaticImageRenderer.h"
#include "util.h"

#include "ThirdParty/lib_Asio.h"

#include <Common/logging.h>
#include <Common/profiling.h>

#include <iostream>
#include <string>
#include <chrono>
#include <array>

namespace ph::cli
{

PH_DEFINE_INTERNAL_LOG_GROUP(Blender, PhotonCLI);

namespace
{

PhFrameSaveInfo make_frame_save_info_for_blender()
{
	// Blender expects specific channel names when loading the render result from .exr file
	/*static const std::array<const PhChar*, 4> channelNames{
		"Combined.R", "Combined.G", "Combined.B", "Combined.A"};*/
	static const std::array<const PhChar*, 4> channelNames{
		"ViewLayer.Combined.R", "ViewLayer.Combined.G", "ViewLayer.Combined.B", "ViewLayer.Combined.A"};

	PhFrameSaveInfo frameInfo{};
	frameInfo.numChannels = channelNames.size();
	frameInfo.channelNames = channelNames.data();

	return frameInfo;
}

}// end anonymous namespace

BlenderStaticImageRenderer::BlenderStaticImageRenderer(const ProcessedArguments& args)

	: StaticImageRenderer(args)

	, m_imageWidthPx(0)
	, m_imageHeightPx(0)
	, m_serverStartPeekingFlag()
{}

void BlenderStaticImageRenderer::render()
{
	// Start server thread early on so Blender can establish connection sooner
	// (server thread will then wait for peeking)
	std::jthread serverThread = makeServerThread(getArgs().getPort(), getArgs().getBlenderPeekInterval());

	setSceneFilePath(getArgs().getSceneFilePath());
	if(!loadCommandsFromSceneFile())
	{
		return;
	}

	phUpdate(getEngine());

	std::thread renderThread([this]()
	{
		PH_PROFILE_NAME_THIS_THREAD("Blender render thread");

		phRender(getEngine());
	});

	phGetRenderDimension(getEngine(), &m_imageWidthPx, &m_imageHeightPx);

	// Stats thread runs right away, create it after render starts
	std::jthread statsThread = makeStatsThread();

	// Notify server thread that it can start peeking
	// (must be after `m_imageWidthPx` and `m_imageHeightPx` are set as server thread needs them)
	m_serverStartPeekingFlag.test_and_set();
	m_serverStartPeekingFlag.notify_one();

	if(renderThread.joinable())
	{
		renderThread.join();
	}
	PH_LOG(Blender, Note, "Render finished.");

	statsThread.request_stop();
	serverThread.request_stop();

	PhUInt64 frameId;
	phCreateFrame(&frameId, m_imageWidthPx, m_imageHeightPx);
	if(getArgs().isPostProcessRequested())
	{
		phAquireFrame(getEngine(), 0, frameId);
	}
	else
	{
		phAquireFrameRaw(getEngine(), 0, frameId);
	}

	const PhFrameSaveInfo frameInfo = make_frame_save_info_for_blender();
	save_frame_with_fail_safe(frameId, getArgs().getImageFilePath(), &frameInfo);

	phDeleteFrame(frameId);
}

std::jthread BlenderStaticImageRenderer::makeStatsThread()
{
	return std::jthread([this](std::stop_token token)
	{
		PH_PROFILE_NAME_THIS_THREAD("Blender stats thread");

		using namespace std::chrono_literals;

		PhFloat32 lastProgress = 0;
		PhFloat32 lastOutputProgress = 0;
		while(!token.stop_requested())
		{
			PhFloat32 currentProgress;
			PhFloat32 samplesPerSecond;
			phAsyncGetRenderStatistics(getEngine(), &currentProgress, &samplesPerSecond);

			if(currentProgress - lastProgress > 1.0f)
			{
				lastProgress = currentProgress;
				std::cout << "progress: " << currentProgress << " % | " 
				          << "samples/sec: " << samplesPerSecond << '\n';
			}

			std::this_thread::sleep_for(10s);
		}
	});
}

std::jthread BlenderStaticImageRenderer::makeServerThread(const uint16 port, const float32 peekIntervalS)
{
	return std::jthread([this, port, peekIntervalS](std::stop_token token)
	{
		PH_PROFILE_NAME_THIS_THREAD("Blender server thread");

		try
		{
			runServer(token, port, peekIntervalS);
		}
		catch(const std::exception& e)
		{
			PH_LOG(Blender, Error, "Error on establishing connection: {}.", e.what());
		}
	});
}

void BlenderStaticImageRenderer::runServer(std::stop_token token, const uint16 port, const float32 peekIntervalS)
{
	const std::chrono::duration<float32> peekInverval(peekIntervalS);
	PH_LOG(Blender, Note, "Server peek interval is {}", peekInverval);

	// At this point, we know nothing but render has not started yet

	asio::io_context ioContext;

	// Server endpoint listening to the specified port
	asio::ip::tcp::endpoint endpoint(asio::ip::tcp::v4(), port);
	asio::ip::tcp::acceptor acceptor(ioContext, endpoint);
	PH_LOG(Blender, Note, "Server listening on port {}", port);

	// A blocking accept, unblocks only if a connection is accepted or an error occurs
	asio::ip::tcp::socket socket(ioContext);
	acceptor.accept(socket);
	PH_LOG(Blender, Note, "Connection accepted.");

	// Start peeking and send data only if being notified
	m_serverStartPeekingFlag.wait(false);

	// Values of `m_imageWidthPx` and `m_imageHeightPx` are synchronized

	PhUInt64 bufferId;
	phCreateBuffer(&bufferId);

	PhUInt64 serverFrameId;
	phCreateFrame(&serverFrameId, m_imageWidthPx, m_imageHeightPx);

	PhRenderProgress currentProgress{};
	PhRenderProgress lastProgress{};

	const PhFrameSaveInfo frameInfo = make_frame_save_info_for_blender();

	while(!token.stop_requested())
	{
		PH_PROFILE_NAMED_SCOPE("Peek and send");

		// When doing peeking, we should strive for faster time to first pixel, and not doing peeking
		// too frequently (computation cost) while keeping the user up-to-date.

		// If nothing progresses, do not bother to peek as it is likely the same result
		lastProgress = currentProgress;
		phAsyncGetRenderProgress(getEngine(), &currentProgress);
		if(currentProgress.totalWork == lastProgress.totalWork &&
		   currentProgress.workDone == lastProgress.workDone)
		{
			// Wait a while before we try to check progress again
			std::this_thread::sleep_for(peekInverval);
			continue;
		}

		phAsyncPeekFrameRaw(getEngine(), 0, 0, 0, m_imageWidthPx, m_imageHeightPx, serverFrameId);
		phSaveFrameToBuffer(serverFrameId, bufferId, PH_BUFFER_FORMAT_EXR_IMAGE, &frameInfo);

		const PhUChar* bytesPtr;
		PhSize numBytes;
		phGetBufferBytes(bufferId, &bytesPtr, &numBytes);

		asio::error_code ignoredError;
		const auto numBytes64 = static_cast<std::uint64_t>(numBytes);
		asio::write(socket, asio::buffer(reinterpret_cast<const unsigned char*>(&numBytes64), 8), ignoredError);
		asio::write(socket, asio::buffer(bytesPtr, numBytes), ignoredError);

		std::this_thread::sleep_for(peekInverval);
	}

	PH_LOG(Blender, Note, "Stopping server...");

	phDeleteBuffer(bufferId);
	phDeleteFrame(serverFrameId);
}

}// end namespace ph::cli
