#include "StaticImageRenderer.h"
#include "CommandLineArguments.h"

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

	// REFACTOR: make a dedicated query class
	std::thread queryThread([&]()
	{
		using namespace std::chrono_literals;
		using Clock = std::chrono::steady_clock;

		const auto startTime = Clock::now();

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

			bool shouldSaveImage = false;
			std::string imageFilePath = m_args.getImageOutputPath() + "_intermediate_";
			if(m_args.getIntervalUnit() == EIntervalUnit::PERCENTAGE)
			{
				if(currentProgress - lastOutputProgress > m_args.getIntermediateOutputInterval())
				{
					shouldSaveImage = true;
					lastOutputProgress = currentProgress;

					if(!m_args.isOverwriteRequested())
					{
						imageFilePath += std::to_string(currentProgress) + "%";
					}

					std::this_thread::sleep_for(2s);
				}
			}
			else if(m_args.getIntervalUnit() == EIntervalUnit::SECOND)
			{
				const auto currentTime = Clock::now();
				const auto duration = currentTime - startTime;
				const auto deltaMs = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();

				shouldSaveImage = true;

				if(!m_args.isOverwriteRequested())
				{
					imageFilePath += std::to_string(deltaMs / 1000.0f) + "s";
				}

				const float fms = m_args.getIntermediateOutputInterval() * 1000;
				const int ims = fms < std::numeric_limits<int>::max() ? static_cast<int>(fms) : std::numeric_limits<int>::max();
				std::this_thread::sleep_for(std::chrono::milliseconds(ims));
			}

			if(shouldSaveImage)
			{
				imageFilePath += "." + m_args.getImageFileFormat();

				if(m_args.isPostProcessRequested())
				{
					phAsyncPeekFrame(m_engineId, 0, 0, 0, filmWpx, filmHpx, queryFrameId);
				}
				else
				{
					phAsyncPeekFrameRaw(m_engineId, 0, 0, 0, filmWpx, filmHpx, queryFrameId);
				}

				phSaveFrame(
					queryFrameId,
					imageFilePath.c_str());
			}
		}// end while
	});

	const bool useServer = m_args.getPort() != 0;

	std::thread serverThread;
	
	if(useServer)
	{
		serverThread = std::thread([&]()
		{
			using asio::ip::tcp;

			const std::string IP = "127.0.0.1";
			const unsigned short PORT = 7000;

			PHuint64 bufferId;
			phCreateBuffer(&bufferId);

			PHuint64 serverFrameId;
			phCreateFrame(&serverFrameId, filmWpx, filmHpx);

			try
			{
				asio::io_context io_context;

				tcp::endpoint endpoint = tcp::endpoint(asio::ip::address::from_string(IP), PORT);
				tcp::acceptor acceptor(io_context, endpoint);

				tcp::socket socket(io_context);
				acceptor.accept(socket);

				while(!isRenderingCompleted)
				{
					phAsyncPeekFrameRaw(m_engineId, 0, 0, 0, filmWpx, filmHpx, serverFrameId);
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

	if(useServer)
	{
		serverThread.join();
	}

	queryThread.join();
}

void StaticImageRenderer::setSceneFilePath(const std::string& path)
{
	m_args.setSceneFilePath(path);

// REFACTOR: use a getReference directory-from-file-path function
#ifndef __APPLE__
	namespace fs = std::filesystem;
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

	return phLoadCommands(m_engineId, sceneFilePath.c_str()) == PH_TRUE ? true : false;
}

PH_CLI_NAMESPACE_END
