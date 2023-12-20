#include "ph_c_core.h"
#include "Api/api_helpers.h"

#include <ph_core.h>
#include <Api/ApiDatabase.h>
#include <Common/assertion.h>
#include <Common/config.h>
#include <Common/logging.h>
#include <Common/profiling.h>
#include <Common/memory.h>
#include <Core/Engine.h>
#include <Core/Receiver/Receiver.h>
#include <Core/Renderer/Renderer.h>
#include <Core/Renderer/RenderRegionStatus.h>
#include <Api/test_scene.h>
#include <Math/TArithmeticArray.h>
#include <DataIO/FileSystem/Path.h>
#include <DataIO/io_utils.h>
#include <Api/ApiHelper.h>
#include <Frame/frame_utils.h>
#include <Frame/TFrame.h>
#include <Frame/PictureMeta.h>
#include <DataIO/io_exceptions.h>
#include <Utility/ByteBuffer.h>
#include <Utility/utility.h>

#include <memory>
#include <iostream>
#include <cstring>
#include <cstddef>
#include <array>

using namespace ph;

PH_DEFINE_INTERNAL_LOG_GROUP(CAPI, Engine);

void phConfigCoreResourceDirectory(const PhChar* const directory)
{
	PH_ASSERT(directory);

	Config::CORE_RESOURCE_DIRECTORY() = std::string(directory);
}

PhBool phInit()
{
	if(!init_render_engine(EngineInitSettings()))
	{
		PH_LOG_ERROR(CAPI, "engine initialization failed");
		return PH_FALSE;
	}

	return PH_TRUE;
}

PhBool phExit()
{
	if(!exit_render_engine())
	{
		PH_LOG_ERROR(CAPI, "engine exiting failed");
		return PH_FALSE;
	}

	return PH_TRUE;
}

void phCreateEngine(PhUInt64* const out_engineId, const PhUInt32 numRenderThreads)
{
	PH_ASSERT(out_engineId);

	auto engine = std::make_unique<Engine>();
	engine->setNumThreads(static_cast<std::size_t>(numRenderThreads));
	*out_engineId = static_cast<PhUInt64>(ApiDatabase::addResource(std::move(engine)));

	PH_LOG(CAPI, "engine<{}> created", *out_engineId);
}

void phSetNumRenderThreads(const PhUInt64 engineId, const PhUInt32 numRenderThreads)
{
	Engine* engine = ApiDatabase::getResource<Engine>(engineId);
	if(engine)
	{
		engine->setNumThreads(static_cast<uint32>(numRenderThreads));
	}
}

void phDeleteEngine(const PhUInt64 engineId)
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

void phEnterCommand(const PhUInt64 engineId, const PhChar* const commandFragment)
{
	static_assert(sizeof(PhChar) == sizeof(char));
	PH_ASSERT(commandFragment);

	Engine* const engine = ApiDatabase::getResource<Engine>(engineId);
	if(engine)
	{
		engine->enterCommand(commandFragment);
	}
}

PhBool phLoadCommands(const PhUInt64 engineId, const PhChar* const filePath)
{
	static_assert(sizeof(PhChar) == sizeof(char));
	PH_ASSERT(filePath);

	Engine* const engine = ApiDatabase::getResource<Engine>(engineId);
	if(engine)
	{
		return engine->loadCommands(Path(filePath)) ? PH_TRUE : PH_FALSE;
	}

	return PH_FALSE;
}

void phRender(const PhUInt64 engineId)
{
	Engine* engine = ApiDatabase::getResource<Engine>(engineId);
	if(engine)
	{
		engine->render();
	}
}

void phUpdate(const PhUInt64 engineId)
{
	Engine* engine = ApiDatabase::getResource<Engine>(engineId);
	if(engine)
	{
		engine->update();
	}
}

void phAquireFrame(
	const PhUInt64 engineId,
	const PhUInt64 channelIndex,
	const PhUInt64 frameId)
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
	const PhUInt64 engineId,
	const PhUInt64 channelIndex,
	const PhUInt64 frameId)
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

void phGetRenderDimension(const PhUInt64 engineId, PhUInt32* const out_widthPx, PhUInt32* const out_heightPx)
{
	Engine* engine = ApiDatabase::getResource<Engine>(engineId);
	if(engine)
	{
		const math::TVector2<int64> dim = engine->getFilmDimensionPx();
		*out_widthPx  = static_cast<PhUInt32>(dim.x());
		*out_heightPx = static_cast<PhUInt32>(dim.y());
	}
}

void phGetObservableRenderData(
	const PhUInt64                       engineId,
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

void phCreateFrame(
	PhUInt64* const out_frameId,
	const PhUInt32 widthPx,
	const PhUInt32 heightPx)
{
	auto frame = std::make_unique<HdrRgbFrame>(widthPx, heightPx);
	*out_frameId = ApiDatabase::addResource(std::move(frame));

	PH_LOG(CAPI, "frame<{}> created", *out_frameId);
}

void phGetFrameDimension(
	const PhUInt64 frameId,
	PhUInt32* const out_widthPx,
	PhUInt32* const out_heightPx)
{
	HdrRgbFrame* frame = ApiDatabase::getResource<HdrRgbFrame>(frameId);
	if(frame)
	{
		*out_widthPx  = static_cast<PhUInt32>(frame->widthPx());
		*out_heightPx = static_cast<PhUInt32>(frame->heightPx());
	}
}

void phGetFrameRgbData(const PhUInt64 frameId, const PhFloat32** const out_data)
{
	HdrRgbFrame* frame = ApiDatabase::getResource<HdrRgbFrame>(frameId);
	if(frame)
	{
		static_assert(sizeof(PhFloat32) == sizeof(HdrComponent));

		*out_data = static_cast<const PhFloat32*>(frame->getPixelData().data());
	}
}

void phDeleteFrame(const PhUInt64 frameId)
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

PhBool phLoadFrame(PhUInt64 frameId, const PhChar* const filePath)
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

PhBool phSaveFrame(
	const PhUInt64 frameId, 
	const PhChar* filePath,
	const PhFrameSaveInfo* saveInfo)
{
	PH_ASSERT(filePath);

	const HdrRgbFrame* frame = ApiDatabase::getResource<HdrRgbFrame>(frameId);
	if(!frame)
	{
		return PH_FALSE;
	}

	try
	{
		if(!saveInfo)
		{
			io_utils::save(*frame, Path(filePath));
		}
		else
		{
			PictureMeta meta = make_picture_meta(*saveInfo);
			io_utils::save(*frame, Path(filePath), &meta);
		}

		return PH_TRUE;
	}
	catch(const FileIOError& e)
	{
		PH_LOG_WARNING(CAPI,
			"frame<{}> saving failed: {}", frameId, e.whatStr());
	}

	return PH_FALSE;
}

PhBool phSaveFrameToBuffer(
	const PhUInt64 frameId,
	const PhUInt64 bufferId,
	const PhBufferFormat format,
	const PhFrameSaveInfo* saveInfo)
{
	PH_PROFILE_SCOPE();

	const HdrRgbFrame* const frame = ApiDatabase::getResource<HdrRgbFrame>(frameId);
	ByteBuffer* const buffer = ApiDatabase::getResource<ByteBuffer>(bufferId);
	if(!frame || !buffer)
	{
		return PH_FALSE;
	}

	buffer->clear();
	if(format == PH_BUFFER_FORMAT_EXR_IMAGE)
	{
		try
		{
			if(!saveInfo)
			{
				io_utils::save_exr(*frame, *buffer);
			}
			else
			{
				PictureMeta meta = make_picture_meta(*saveInfo);
				io_utils::save_exr(*frame, *buffer, &meta);
			}
			
			return PH_TRUE;
		}
		catch(const Exception& e)
		{
			PH_LOG_ERROR(CAPI, "frame<{}> saving failed: {}", frameId, e.what());
		}
	}
	else if(format == PH_BUFFER_FORMAT_FLOAT32_ARRAY)
	{
		constexpr auto maxChannels = HdrRgbFrame::PixelType::NUM_ELEMENTS;

		std::array<bool, maxChannels> shouldSaveChannel;
		for(std::size_t channelIdx = 0; channelIdx < maxChannels; ++channelIdx)
		{
			shouldSaveChannel[channelIdx] = true;

			if(saveInfo)
			{
				// Skip the channel if we specify less channels or the channel name is empty
				if(channelIdx >= saveInfo->numChannels ||
				   (saveInfo->channelNames && (!saveInfo->channelNames[channelIdx] ||
				                                saveInfo->channelNames[channelIdx][0] == '\0')))
				{
					shouldSaveChannel[channelIdx] = false;
				}
			}
		}

		const bool needReversingBytes = saveInfo ? is_reversing_bytes_needed(saveInfo->endianness) : false;

		frame->forEachPixel(
			[&shouldSaveChannel, needReversingBytes, buffer](const HdrRgbFrame::PixelType& pixel)
			{
				for(std::size_t channelIdx = 0; channelIdx < maxChannels; ++channelIdx)
				{
					if(!shouldSaveChannel[channelIdx])
					{
						continue;
					}

					const auto pixelComponent = pixel[channelIdx];

					std::array<std::byte, sizeof(pixelComponent)> bytes;
					to_bytes(pixelComponent, bytes.data());
					if(needReversingBytes)
					{
						reverse_bytes<sizeof(pixelComponent)>(bytes.data());
					}

					buffer->write(bytes.data(), bytes.size());
				}
			});

		return PH_TRUE;
	}
	else
	{
		PH_LOG_ERROR(CAPI, 
			"cannot save frame<{}> in unknown format {}", frameId, enum_to_value(format));
	}

	return PH_FALSE;
}

void phFrameOpAbsDifference(const PhUInt64 frameAId, const PhUInt64 frameBId, const PhUInt64 resultFrameId)
{
	HdrRgbFrame* frameA      = ApiDatabase::getResource<HdrRgbFrame>(frameAId);
	HdrRgbFrame* frameB      = ApiDatabase::getResource<HdrRgbFrame>(frameBId);
	HdrRgbFrame* resultFrame = ApiDatabase::getResource<HdrRgbFrame>(resultFrameId);
	if(frameA && frameB && resultFrame)
	{
		frame_utils::abs_diff(*frameA, *frameB, resultFrame);
	}
}

PhFloat32 phFrameOpMSE(const PhUInt64 expectedFrameId, const PhUInt64 estimatedFrameId)
{
	PhFloat32 MSE = 0.0f;

	HdrRgbFrame* expectedFrame  = ApiDatabase::getResource<HdrRgbFrame>(expectedFrameId);
	HdrRgbFrame* estimatedFrame = ApiDatabase::getResource<HdrRgbFrame>(estimatedFrameId);
	if(expectedFrame && estimatedFrame)
	{
		MSE = static_cast<PhFloat32>(frame_utils::calc_MSE(*expectedFrame, *estimatedFrame));
	}
	else
	{
		PH_LOG_WARNING(CAPI, "phFrameOpMSE(2) returned 0 due to invalid frame");
	}

	return MSE;
}

void phAsyncGetRendererStatistics(
	const PhUInt64 engineId,
	PhFloat32* const out_percentageProgress,
	PhFloat32* const out_samplesPerSecond)
{
	auto engine = ApiDatabase::useResource<Engine>(engineId).lock();
	if(engine)
	{
		float32 percentageProgress, samplesPerSecond;
		engine->asyncQueryStatistics(&percentageProgress, &samplesPerSecond);

		*out_percentageProgress = static_cast<PhFloat32>(percentageProgress);
		*out_samplesPerSecond = static_cast<PhFloat32>(samplesPerSecond);
	}
}

void phAsyncGetRendererState(
	const PhUInt64              engineId,
	struct PHRenderState* const out_state)
{
	PH_ASSERT(out_state);

	auto engine = ApiDatabase::useResource<Engine>(engineId).lock();
	if(engine)
	{
		const RenderStats state = engine->getRenderer()->asyncQueryRenderStats();

		for(std::size_t i = 0; i < PH_NUM_RENDER_STATE_INTEGERS; ++i)
		{
			out_state->integers[i] = static_cast<PhInt64>(state.getInteger(i));
		}
		for(std::size_t i = 0; i < PH_NUM_RENDER_STATE_REALS; ++i)
		{
			out_state->reals[i] = static_cast<PhFloat32>(state.getReal(i));
		}
	}
}

PhBool phAsyncPollUpdatedFrameRegion(
	PhUInt64 engineId,
	PhFrameRegionInfo* out_regionInfo)
{
	PH_PROFILE_SCOPE();

	auto engine = ApiDatabase::useResource<Engine>(engineId).lock();
	if(!engine || !engine->getRenderer())
	{
		return PH_FALSE;
	}

	RenderRegionStatus region;
	const auto numRegions = engine->getRenderer()->asyncPollUpdatedRegions(TSpan{&region, 1});
	if(numRegions == 0)
	{
		return PH_FALSE;
	}

	to_frame_region_info(region, out_regionInfo);

	return PH_TRUE;
}

PhSize phAsyncPollUpdatedFrameRegions(
	PhUInt64 engineId,
	PhUInt64 bufferId,
	PhFrameRegionInfo* out_regionInfos,
	PhSize maxRegionInfos)
{
	PH_PROFILE_SCOPE();

	auto engine = ApiDatabase::useResource<Engine>(engineId).lock();
	auto buffer = ApiDatabase::useResource<ByteBuffer>(bufferId).lock();
	if(!engine || !engine->getRenderer() || !buffer)
	{
		return 0;
	}

	buffer->rewindWrite();
	TSpan<RenderRegionStatus> regions = make_array_from_buffer<RenderRegionStatus>(
		maxRegionInfos, *buffer, true);

	const auto numPolledRegions = engine->getRenderer()->asyncPollUpdatedRegions(regions);

	// Translate from `RenderRegionStatus` to `PhFrameRegionInfo`
	for(std::size_t ri = 0; ri < numPolledRegions; ++ri)
	{
		to_frame_region_info(regions[ri], out_regionInfos + ri);
	}

	return numPolledRegions;
}

PhSize phAsyncPollMergedUpdatedFrameRegions(
	PhUInt64 engineId,
	PhUInt64 bufferId,
	PhSize mergeSize,
	PhFrameRegionInfo* out_regionInfos,
	PhSize maxRegionInfos)
{
	PH_PROFILE_SCOPE();

	auto engine = ApiDatabase::useResource<Engine>(engineId).lock();
	auto buffer = ApiDatabase::useResource<ByteBuffer>(bufferId).lock();
	if(!engine || !engine->getRenderer() || !buffer)
	{
		return 0;
	}

	buffer->rewindWrite();
	TSpan<RenderRegionStatus> regions = make_array_from_buffer<RenderRegionStatus>(
		maxRegionInfos, *buffer, true);

	const auto numPolledRegions = engine->getRenderer()->asyncPollMergedUpdatedRegions(regions, mergeSize);

	// Translate from `RenderRegionStatus` to `PhFrameRegionInfo`
	for(std::size_t ri = 0; ri < numPolledRegions; ++ri)
	{
		to_frame_region_info(regions[ri], out_regionInfos + ri);
	}

	return numPolledRegions;
}

void phAsyncPeekFrame(
	const PhUInt64 engineId,
	const PhUInt64 channelIndex,
	const PhUInt32 xPx,
	const PhUInt32 yPx,
	const PhUInt32 widthPx,
	const PhUInt32 heightPx,
	const PhUInt64 frameId)
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
	const PhUInt64 engineId,
	const PhUInt64 channelIndex,
	const PhUInt32 xPx,
	const PhUInt32 yPx,
	const PhUInt32 widthPx,
	const PhUInt32 heightPx,
	const PhUInt64 frameId)
{
	auto engine = ApiDatabase::useResource<Engine>(engineId).lock();
	auto frame  = ApiDatabase::useResource<HdrRgbFrame>(frameId).lock();
	if(engine && frame)
	{
		Region region({xPx, yPx}, {xPx + widthPx, yPx + heightPx});
		engine->asyncPeekFrame(channelIndex, region, *frame, false);
	}
}

void phSetWorkingDirectory(const PhUInt64 engineId, const PhChar* const workingDirectory)
{
	static_assert(sizeof(PhChar) == sizeof(char));

	Engine* engine = ApiDatabase::getResource<Engine>(engineId);
	if(engine)
	{
		const Path path(workingDirectory);
		engine->setWorkingDirectory(path);
	}
}

void phCreateBuffer(PhUInt64* const out_bufferId)
{
	PH_ASSERT(out_bufferId);

	*out_bufferId = static_cast<PhUInt64>(ApiDatabase::addResource(std::make_unique<ByteBuffer>()));

	PH_LOG(CAPI, "buffer<{}> created", *out_bufferId);
}

void phGetBufferBytes(
	const PhUInt64 bufferId, 
	const PhUChar** const out_bytesPtr, 
	PhSize* const out_numBytes)
{
	static_assert(sizeof(PhUChar) == sizeof(std::byte));
	PH_ASSERT(out_bytesPtr);
	PH_ASSERT(out_numBytes);

	ByteBuffer* const buffer = ApiDatabase::getResource<ByteBuffer>(bufferId);
	if(buffer)
	{
		*out_bytesPtr = reinterpret_cast<const unsigned char*>(buffer->getBytes().data());
		*out_numBytes = static_cast<PhSize>(buffer->getBytes().size());
	}
}

void phDeleteBuffer(const PhUInt64 bufferId)
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
