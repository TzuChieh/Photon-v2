#include "ph_core.h"
#include "Api/ApiDatabase.h"
#include "Core/Engine.h"
#include "Core/Receiver/Receiver.h"
#include "Api/test_scene.h"
#include "Frame/TFrame.h"
#include "Math/TArithmeticArray.h"
#include "ph_cpp_core.h"
#include "Core/Renderer/Renderer.h"
#include "DataIO/FileSystem/Path.h"
#include "Common/assertion.h"
#include "DataIO/io_utils.h"
#include "Api/ApiHelper.h"
#include "Core/Scheduler/Region.h"
#include "Common/config.h"
#include "Common/logging.h"
#include "Frame/frame_utils.h"
#include "Utility/ByteBuffer.h"
#include "DataIO/io_exceptions.h"

#include <memory>
#include <iostream>
#include <cstring>
#include <cstddef>

using namespace ph;

PH_DEFINE_INTERNAL_LOG_GROUP(CAPI, Engine);

void phConfigCoreResourceDirectory(const PHchar* const directory)
{
	PH_ASSERT(directory);

	Config::CORE_RESOURCE_DIRECTORY() = std::string(directory);
}

int phInit()
{
	if(!init_render_engine(EngineInitSettings()))
	{
		PH_LOG_ERROR(CAPI, "engine initialization failed");
		return PH_FALSE;
	}

	return PH_TRUE;
}

int phExit()
{
	if(!exit_render_engine())
	{
		PH_LOG_ERROR(CAPI, "engine exiting failed");
		return PH_FALSE;
	}

	return PH_TRUE;
}

void phCreateEngine(PHuint64* const out_engineId, const PHuint32 numRenderThreads)
{
	PH_ASSERT(out_engineId);

	auto engine = std::make_unique<Engine>();
	engine->setNumRenderThreads(static_cast<std::size_t>(numRenderThreads));
	*out_engineId = static_cast<PHuint64>(ApiDatabase::addResource(std::move(engine)));

	PH_LOG(CAPI, "engine<{}> created", *out_engineId);
}

void phSetNumRenderThreads(const PHuint64 engineId, const PHuint32 numRenderThreads)
{
	Engine* engine = ApiDatabase::getResource<Engine>(engineId);
	if(engine)
	{
		engine->setNumRenderThreads(static_cast<uint32>(numRenderThreads));
	}
}

void phDeleteEngine(const PHuint64 engineId)
{
	if(ApiDatabase::removeResource<Engine>(engineId))
	{
		PH_LOG(CAPI, "engine<{}> deleted", engineId);
	}
	else
	{
		PH_LOG_WARNING(CAPI, "error while deleting engine<{}>", engineId);
	}
}

void phEnterCommand(const PHuint64 engineId, const PHchar* const commandFragment)
{
	static_assert(sizeof(PHchar) == sizeof(char));
	PH_ASSERT(commandFragment);

	Engine* const engine = ApiDatabase::getResource<Engine>(engineId);
	if(engine)
	{
		engine->enterCommand(commandFragment);
	}
}

int phLoadCommands(const PHuint64 engineId, const PHchar* const filePath)
{
	static_assert(sizeof(PHchar) == sizeof(char));
	PH_ASSERT(filePath);

	Engine* const engine = ApiDatabase::getResource<Engine>(engineId);
	if(engine)
	{
		return engine->loadCommands(Path(filePath)) ? PH_TRUE : PH_FALSE;
	}

	return PH_FALSE;
}

void phRender(const PHuint64 engineId)
{
	Engine* engine = ApiDatabase::getResource<Engine>(engineId);
	if(engine)
	{
		engine->render();
	}
}

void phUpdate(const PHuint64 engineId)
{
	Engine* engine = ApiDatabase::getResource<Engine>(engineId);
	if(engine)
	{
		engine->update();
	}
}

void phAquireFrame(
	const PHuint64 engineId,
	const PHuint64 channelIndex,
	const PHuint64 frameId)
{
	Engine*      engine = ApiDatabase::getResource<Engine>(engineId);
	HdrRgbFrame* frame  = ApiDatabase::getResource<HdrRgbFrame>(frameId);
	if(engine && frame)
	{
		engine->retrieveFrame(
			static_cast<std::size_t>(channelIndex), 
			*frame);
	}
}

void phAquireFrameRaw(
	const PHuint64 engineId,
	const PHuint64 channelIndex,
	const PHuint64 frameId)
{
	Engine*      engine = ApiDatabase::getResource<Engine>(engineId);
	HdrRgbFrame* frame  = ApiDatabase::getResource<HdrRgbFrame>(frameId);
	if(engine && frame)
	{
		engine->retrieveFrame(
			static_cast<std::size_t>(channelIndex),
			*frame, 
			false);
	}
}

void phGetRenderDimension(const PHuint64 engineId, PHuint32* const out_widthPx, PHuint32* const out_heightPx)
{
	Engine* engine = ApiDatabase::getResource<Engine>(engineId);
	if(engine)
	{
		const math::TVector2<int64> dim = engine->getFilmDimensionPx();
		*out_widthPx  = static_cast<PHuint32>(dim.x());
		*out_heightPx = static_cast<PHuint32>(dim.y());
	}
}

void phGetObservableRenderData(
	const PHuint64                       engineId,
	struct PHObservableRenderData* const out_data)
{
	PH_ASSERT(out_data);

	Engine* engine = ApiDatabase::getResource<Engine>(engineId);
	if(engine)
	{
		const auto data = engine->getRenderer()->getObservationInfo();

		for(std::size_t i = 0; i < PH_NUM_RENDER_LAYERS; ++i)
		{
			out_data->layers[i][0] = '\0';
			if(i < data.numLayers())
			{
				std::strncpy(
					out_data->layers[i], 
					data.getLayerName(i).c_str(), 
					PH_MAX_NAME_LENGTH);
				out_data->layers[i][PH_MAX_NAME_LENGTH] = '\0';
			}
		}

		for(std::size_t i = 0; i < PH_NUM_RENDER_STATE_INTEGERS; ++i)
		{
			out_data->integers[i][0] = '\0';
			if(i < data.numIntegerStats())
			{
				std::strncpy(
					out_data->integers[i],
					data.getIntegerStatName(i).c_str(),
					PH_MAX_NAME_LENGTH);
				out_data->integers[i][PH_MAX_NAME_LENGTH] = '\0';
			}
		}

		for(std::size_t i = 0; i < PH_NUM_RENDER_STATE_REALS; ++i)
		{
			out_data->reals[i][0] = '\0';
			if(i < data.numRealStats())
			{
				std::strncpy(
					out_data->reals[i],
					data.getRealStatName(i).c_str(),
					PH_MAX_NAME_LENGTH);
				out_data->reals[i][PH_MAX_NAME_LENGTH] = '\0';
			}
		}
	}
}

void phCreateFrame(PHuint64* const out_frameId,
                   const PHuint32 widthPx, const PHuint32 heightPx)
{
	auto frame = std::make_unique<HdrRgbFrame>(widthPx, heightPx);
	*out_frameId = ApiDatabase::addResource(std::move(frame));

	PH_LOG(CAPI, "frame<{}> created", *out_frameId);
}

void phGetFrameDimension(const PHuint64 frameId, 
                         PHuint32* const out_widthPx, PHuint32* const out_heightPx)
{
	HdrRgbFrame* frame = ApiDatabase::getResource<HdrRgbFrame>(frameId);
	if(frame)
	{
		*out_widthPx  = static_cast<PHuint32>(frame->widthPx());
		*out_heightPx = static_cast<PHuint32>(frame->heightPx());
	}
}

void phGetFrameRgbData(const PHuint64 frameId, const PHfloat32** const out_data)
{
	HdrRgbFrame* frame = ApiDatabase::getResource<HdrRgbFrame>(frameId);
	if(frame)
	{
		static_assert(sizeof(PHfloat32) == sizeof(HdrComponent));

		*out_data = static_cast<const PHfloat32*>(frame->getPixelData().data());
	}
}

void phDeleteFrame(const PHuint64 frameId)
{
	if(ApiDatabase::removeResource<HdrRgbFrame>(frameId))
	{
		PH_LOG(CAPI, "frame<{}> deleted", frameId);
	}
	else
	{
		PH_LOG_WARNING(CAPI, "error while deleting frame<{}>", frameId);
	}
}

int phLoadFrame(PHuint64 frameId, const PHchar* const filePath)
{
	PH_ASSERT(filePath);

	/*HdrRgbFrame* frame = ApiDatabase::getResource<HdrRgbFrame>(frameId);
	if(frame)
	{
		*frame = io_utils::load_picture(Path(filePath)).frame;
		return PH_TRUE;
	}
	else
	{
		return PH_FALSE;
	}*/
	PH_ASSERT_UNREACHABLE_SECTION();
	return PH_FALSE;
}

int phSaveFrame(const PHuint64 frameId, const PHchar* const filePath)
{
	PH_ASSERT(filePath);

	const HdrRgbFrame* frame = ApiDatabase::getResource<HdrRgbFrame>(frameId);
	if(frame)
	{
		try
		{
			io_utils::save(*frame, Path(filePath));
			return PH_TRUE;
		}
		catch(const FileIOError& e)
		{
			PH_LOG_WARNING(CAPI,
				"frame<{}> saving failed: {}", 
				frameId, e.whatStr());
			return PH_FALSE;
		}
	}

	return PH_FALSE;
}

int phSaveFrameToBuffer(const PHuint64 frameId, const PHuint64 bufferId)
{
	const HdrRgbFrame* const frame  = ApiDatabase::getResource<HdrRgbFrame>(frameId);
	ByteBuffer* const        buffer = ApiDatabase::getResource<ByteBuffer>(bufferId);

	if(!frame || !buffer)
	{
		return PH_FALSE;
	}

	std::string buf;
	try
	{
		io_utils::save_exr(*frame, buf);
	}
	catch(const FileIOError& e)
	{
		PH_LOG_WARNING(CAPI,
			"frame<{}> saving failed: {}", 
			frameId, e.whatStr());
		return PH_FALSE;
	}

	buffer->clear();
	buffer->write(buf.data(), buf.size());

	return PH_TRUE;
}

void phFrameOpAbsDifference(const PHuint64 frameAId, const PHuint64 frameBId, const PHuint64 resultFrameId)
{
	HdrRgbFrame* frameA      = ApiDatabase::getResource<HdrRgbFrame>(frameAId);
	HdrRgbFrame* frameB      = ApiDatabase::getResource<HdrRgbFrame>(frameBId);
	HdrRgbFrame* resultFrame = ApiDatabase::getResource<HdrRgbFrame>(resultFrameId);
	if(frameA && frameB && resultFrame)
	{
		frame_utils::abs_diff(*frameA, *frameB, resultFrame);
	}
}

float phFrameOpMSE(const PHuint64 expectedFrameId, const PHuint64 estimatedFrameId)
{
	float MSE = 0.0f;

	HdrRgbFrame* expectedFrame  = ApiDatabase::getResource<HdrRgbFrame>(expectedFrameId);
	HdrRgbFrame* estimatedFrame = ApiDatabase::getResource<HdrRgbFrame>(estimatedFrameId);
	if(expectedFrame && estimatedFrame)
	{
		MSE = static_cast<float>(frame_utils::calc_MSE(*expectedFrame, *estimatedFrame));
	}
	else
	{
		PH_LOG_WARNING(CAPI, "phFrameOpMSE(2) returned 0 due to invalid frame");
	}

	return MSE;
}

void phAsyncGetRendererStatistics(const PHuint64 engineId,
                                  PHfloat32* const out_percentageProgress,
                                  PHfloat32* const out_samplesPerSecond)
{
	auto engine = ApiDatabase::useResource<Engine>(engineId).lock();
	if(engine)
	{
		float32 percentageProgress, samplesPerSecond;
		engine->asyncQueryStatistics(&percentageProgress, &samplesPerSecond);

		*out_percentageProgress = static_cast<PHfloat32>(percentageProgress);
		*out_samplesPerSecond = static_cast<PHfloat32>(samplesPerSecond);
	}
}

void phAsyncGetRendererState(
	const PHuint64              engineId,
	struct PHRenderState* const out_state)
{
	PH_ASSERT(out_state);

	auto engine = ApiDatabase::useResource<Engine>(engineId).lock();
	if(engine)
	{
		const RenderStats state = engine->getRenderer()->asyncQueryRenderStats();

		for(std::size_t i = 0; i < PH_NUM_RENDER_STATE_INTEGERS; ++i)
		{
			out_state->integers[i] = static_cast<PHint64>(state.getInteger(i));
		}
		for(std::size_t i = 0; i < PH_NUM_RENDER_STATE_REALS; ++i)
		{
			out_state->reals[i] = static_cast<PHfloat32>(state.getReal(i));
		}
	}
}

int phAsyncPollUpdatedFrameRegion(
	const PHuint64  engineId,
	PHuint32* const out_xPx,
	PHuint32* const out_yPx,
	PHuint32* const out_widthPx,
	PHuint32* const out_heightPx)
{
	PH_ASSERT(out_xPx && out_yPx && out_widthPx && out_heightPx);

	auto engine = ApiDatabase::useResource<Engine>(engineId).lock();
	if(engine)
	{
		Region region;
		const ERegionStatus status = engine->asyncPollUpdatedRegion(&region);

		*out_xPx      = static_cast<PHuint32>(region.getMinVertex().x());
		*out_yPx      = static_cast<PHuint32>(region.getMinVertex().y());
		*out_widthPx  = static_cast<PHuint32>(region.getWidth());
		*out_heightPx = static_cast<PHuint32>(region.getHeight());

		switch(status)
		{
		case ERegionStatus::INVALID:  return PH_FILM_REGION_STATUS_INVALID;
		case ERegionStatus::UPDATING: return PH_FILM_REGION_STATUS_UPDATING;
		case ERegionStatus::FINISHED: return PH_FILM_REGION_STATUS_FINISHED;
		}
	}

	return PH_FILM_REGION_STATUS_INVALID;
}

void phAsyncPeekFrame(
	const PHuint64 engineId,
	const PHuint64 channelIndex,
	const PHuint32 xPx,
	const PHuint32 yPx,
	const PHuint32 widthPx,
	const PHuint32 heightPx,
	const PHuint64 frameId)
{
	auto engine = ApiDatabase::useResource<Engine>(engineId).lock();
	auto frame  = ApiDatabase::useResource<HdrRgbFrame>(frameId).lock();
	if(engine && frame)
	{
		Region region({xPx, yPx}, {xPx + widthPx, yPx + heightPx});
		engine->asyncPeekFrame(channelIndex, region, *frame);
	}
}

void phAsyncPeekFrameRaw(
	const PHuint64 engineId,
	const PHuint64 channelIndex,
	const PHuint32 xPx,
	const PHuint32 yPx,
	const PHuint32 widthPx,
	const PHuint32 heightPx,
	const PHuint64 frameId)
{
	auto engine = ApiDatabase::useResource<Engine>(engineId).lock();
	auto frame  = ApiDatabase::useResource<HdrRgbFrame>(frameId).lock();
	if(engine && frame)
	{
		Region region({xPx, yPx}, {xPx + widthPx, yPx + heightPx});
		engine->asyncPeekFrame(channelIndex, region, *frame, false);
	}
}

void phSetWorkingDirectory(const PHuint64 engineId, const PHchar* const workingDirectory)
{
	static_assert(sizeof(PHchar) == sizeof(char));

	Engine* engine = ApiDatabase::getResource<Engine>(engineId);
	if(engine)
	{
		const Path path(workingDirectory);
		engine->setWorkingDirectory(path);
	}
}

void phCreateBuffer(PHuint64* const out_bufferId)
{
	PH_ASSERT(out_bufferId);

	*out_bufferId = static_cast<PHuint64>(ApiDatabase::addResource(std::make_unique<ByteBuffer>()));

	PH_LOG(CAPI, "buffer<{}> created", *out_bufferId);
}

void phGetBufferBytes(const PHuint64 bufferId, const unsigned char** const out_bytesPtr, size_t* const out_numBytes)
{
	static_assert(sizeof(unsigned char) == sizeof(std::byte));
	PH_ASSERT(out_bytesPtr);
	PH_ASSERT(out_numBytes);

	ByteBuffer* const buffer = ApiDatabase::getResource<ByteBuffer>(bufferId);
	if(buffer)
	{
		*out_bytesPtr = reinterpret_cast<const unsigned char*>(buffer->getDataPtr());
		*out_numBytes = static_cast<size_t>(buffer->numBytes());
	}
}

void phDeleteBuffer(const PHuint64 bufferId)
{
	if(ApiDatabase::removeResource<ByteBuffer>(bufferId))
	{
		PH_LOG(CAPI, "buffer<{}> deleted", bufferId);
	}
	else
	{
		PH_LOG_WARNING(CAPI, "error while deleting buffer<{}>", bufferId);
	}
}
