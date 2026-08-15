#include "BlenderStaticImageRenderer.h"
#include "BlenderFrameDataView.h"
#include "util.h"

#include "ThirdParty/lib_Asio.h"

#include <Common/logging.h>
#include <Common/profiling.h>

#include <array>
#include <chrono>
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

namespace ph::cli
{

PH_DEFINE_INTERNAL_LOG_GROUP(Blender, PhotonCLI);

namespace
{

PhFrameSaveInfo make_frame_save_info_for_blender()
{
	// PhotonBlend loads EXR into the active Blender render layer
	static const std::array<const PhChar*, 4> channelNames{
		"Combined.R", "Combined.G", "Combined.B", "Combined.A"};

	PhFrameSaveInfo frameInfo{};
	frameInfo.numChannels = channelNames.size();
	frameInfo.channelNames = channelNames.data();

	return frameInfo;
}

void send_frame_data(
	asio::ip::tcp::socket& socket,
	const BlenderFrameDataView& frameDataView,
	std::vector<asio::const_buffer>& buffer)
{
	buffer.clear();

	const auto headerBytes = frameDataView.getHeaderBytes();
	buffer.push_back(asio::buffer(headerBytes.data(), headerBytes.size()));
	for(uint32 rowIndex = 0; rowIndex < frameDataView.numRows(); ++rowIndex)
	{
		const auto rowBytes = frameDataView.getRowBytes(rowIndex);
		buffer.push_back(asio::buffer(rowBytes.data(), rowBytes.size()));
	}

	asio::write(socket, buffer);
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
		// Something must be wrong and we cannot recover from this
		std::exit(EXIT_FAILURE);
	}

	phUpdate(getSession());

	std::thread renderThread([this]()
	{
		PH_PROFILE_NAME_THIS_THREAD("Blender render thread");

		phRender(getSession());
	});

	phGetRenderDimension(getSession(), &m_imageWidthPx, &m_imageHeightPx);

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
		phRetrieveFrame(getSession(), 0, frameId);
	}
	else
	{
		phRetrieveFrameRaw(getSession(), 0, frameId);
	}

	const PhFrameSaveInfo frameInfo = make_frame_save_info_for_blender();
	save_frame_with_fail_safe(frameId, getArgs().getImageFilePath(0, 1), &frameInfo);

	phDeleteFrame(frameId);
}

std::jthread BlenderStaticImageRenderer::makeStatsThread()
{
	return std::jthread([this](std::stop_token token)
	{
		PH_PROFILE_NAME_THIS_THREAD("Blender stats thread");

		using namespace std::chrono_literals;

		PhFloat32 lastProgress = 0;
		while(!token.stop_requested())
		{
			PhFloat32 currentProgress;
			PhFloat32 samplesPerSecond;
			phAsyncGetRenderStatistics(getSession(), &currentProgress, &samplesPerSecond);

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
			PH_LOG(Blender, Error, "rendering server failed: {}.", e.what());

			// We do not support canceling an active `phRender()` cooperatively
			std::exit(EXIT_FAILURE);
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

	PhUInt64 regionBufferId;
	phCreateBuffer(&regionBufferId);

	constexpr std::size_t maxRegionUpdatesPerPoll = 64;
	std::array<PhFrameRegionInfo, maxRegionUpdatesPerPoll> updatedRegions{};

	PhUInt64 serverFrameId;
	phCreateFrame(&serverFrameId, m_imageWidthPx, m_imageHeightPx);

	const PhFloat32* frameRgbData;
	phGetFrameRgbData(serverFrameId, &frameRgbData);

	std::vector<asio::const_buffer> sendBuffer;
	sendBuffer.reserve(128 * 128 * 4 * 4);

	// We should strive for faster time to first pixel, and not doing peeking
	// too frequently (computation cost) while keeping the user up-to-date.
	while(!token.stop_requested())
	{
		PH_PROFILE_NAMED_SCOPE("Peek and send");

		const PhSize numUpdatedRegions = phAsyncPollUpdatedFrameRegions(
			getSession(), regionBufferId, updatedRegions.data(), updatedRegions.size());

		for(PhSize regionIndex = 0; regionIndex < numUpdatedRegions; ++regionIndex)
		{
			const PhFrameRegionInfo& region = updatedRegions[regionIndex];
			phAsyncPeekFrameRaw(
				getSession(),
				0,
				region.xPx,
				region.yPx,
				region.widthPx,
				region.heightPx,
				serverFrameId);

			constexpr uint32 numFrameChannels = 3;
			const BlenderFrameDataView frameDataView(
				frameRgbData,
				m_imageWidthPx,
				m_imageHeightPx,
				numFrameChannels,
				region);
			send_frame_data(socket, frameDataView, sendBuffer);
		}// end for each updated region

		if(numUpdatedRegions < updatedRegions.size())
		{
			std::this_thread::sleep_for(peekInverval);
		}
	}

	const auto endHeaderBytes = BlenderFrameDataView::getEndHeaderBytes();
	asio::write(socket, asio::buffer(endHeaderBytes.data(), endHeaderBytes.size()));

	PH_LOG(Blender, Note, "Stopping server...");

	phDeleteFrame(serverFrameId);
	phDeleteBuffer(regionBufferId);
}

}// end namespace ph::cli
