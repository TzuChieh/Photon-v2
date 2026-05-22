#include "CEngine/ph_c_core.h"
#include "CEngine/api_helpers.h"

#include <Engine/ph_core.h>
#include <Engine/ApiDatabase.h>
#include <Common/assertion.h>
#include <Common/config.h>
#include <Common/logging.h>
#include <Common/profiling.h>
#include <Common/memory.h>
#include <Engine/Core/Engine.h>
#include <Engine/Core/Receiver/Receiver.h>
#include <Engine/Core/Renderer/Renderer.h>
#include <Engine/Core/Renderer/RenderRegionStatus.h>
#include <Engine/test_scene.h>
#include <Engine/Math/TArithmeticArray.h>
#include <Engine/DataIO/FileSystem/Path.h>
#include <Engine/DataIO/io_utils.h>
#include <Engine/Frame/frame_utils.h>
#include <Engine/Frame/TFrame.h>
#include <Engine/Frame/PictureMeta.h>
#include <Common/io_exceptions.h>
#include <Engine/Utility/ByteBuffer.h>
#include <Engine/Utility/utility.h>

#include <memory>
#include <iostream>
#include <cstring>
#include <cstddef>
#include <array>
#include <string>

using namespace ph;

PH_DEFINE_INTERNAL_LOG_GROUP(CAPI, Engine);

namespace
{

void copy_name_to_buffer(
	const std::string& name,
	PhChar* const out_name,
	PhSize* const out_nameLength)
{
	const std::size_t requiredSize = name.size() + 1;
	if(out_nameLength)
	{
		*out_nameLength = requiredSize;
	}

	if(out_name)
	{
		std::memcpy(out_name, name.data(), requiredSize - 1);
		out_name[requiredSize - 1] = '\0';
	}
}

bool get_render_observation_info(
	const PhUInt64 sessionId,
	RenderObservationInfo* const out_info)
{
	PH_ASSERT(out_info);

	Engine* const engine = ApiDatabase::getResource<Engine>(sessionId);
	if(!engine || !engine->getRenderer())
	{
		return false;
	}

	*out_info = engine->getRenderer()->getObservationInfo();
	return true;
}

}// end anonymous namespace

PhResult phInit()
{
	if(!init_render_engine())
	{
		std::cerr << "engine initializing failed\n";
		return PH_ERROR_OPERATION_FAILED;
	}

	return PH_OK;
}

PhResult phExit()
{
	if(!exit_render_engine())
	{
		std::cerr << "engine exiting failed\n";
		return PH_ERROR_OPERATION_FAILED;
	}

	return PH_OK;
}

void phCreateSession(PhUInt64* const out_sessionId, const PhUInt32 numRenderThreads)
{
	PH_ASSERT(out_sessionId);

	auto engine = std::make_unique<Engine>();
	engine->setNumThreads(static_cast<std::size_t>(numRenderThreads));
	*out_sessionId = static_cast<PhUInt64>(ApiDatabase::addResource(std::move(engine)));

	PH_LOG(CAPI, Note, "session<{}> created", *out_sessionId);
}

void phSetNumRenderThreads(const PhUInt64 sessionId, const PhUInt32 numRenderThreads)
{
	Engine* engine = ApiDatabase::getResource<Engine>(sessionId);
	if(engine)
	{
		engine->setNumThreads(static_cast<uint32>(numRenderThreads));
	}
}

void phDeleteSession(const PhUInt64 sessionId)
{
	if(ApiDatabase::removeResource<Engine>(sessionId))
	{
		PH_LOG(CAPI, Note, "session<{}> deleted", sessionId);
	}
	else
	{
		PH_LOG(CAPI, Warning, "error while deleting session<{}>", sessionId);
	}
}

void phEnterCommand(const PhUInt64 sessionId, const PhChar* const commandFragment)
{
	static_assert(sizeof(PhChar) == sizeof(char));
	PH_ASSERT(commandFragment);

	Engine* const engine = ApiDatabase::getResource<Engine>(sessionId);
	if(engine)
	{
		engine->enterCommand(commandFragment);
	}
}

PhResult phLoadCommands(const PhUInt64 sessionId, const PhChar* const filePath)
{
	static_assert(sizeof(PhChar) == sizeof(char));
	if(!filePath)
	{
		return PH_ERROR_INVALID_ARGUMENT;
	}

	Engine* const engine = ApiDatabase::getResource<Engine>(sessionId);
	if(engine)
	{
		return engine->loadCommands(Path(filePath))
			? PH_OK
			: PH_ERROR_OPERATION_FAILED;
	}

	return PH_ERROR_NOT_FOUND;
}

void phRender(const PhUInt64 sessionId)
{
	Engine* engine = ApiDatabase::getResource<Engine>(sessionId);
	if(engine)
	{
		engine->render();
	}
}

void phUpdate(const PhUInt64 sessionId)
{
	Engine* engine = ApiDatabase::getResource<Engine>(sessionId);
	if(engine)
	{
		engine->update();
	}
}

PhResult phRetrieveFrame(
	const PhUInt64 sessionId,
	const PhInt32 layerIndex,
	const PhUInt64 frameId)
{
	if(layerIndex < 0)
	{
		PH_LOG(CAPI, Warning, "invalid layer index {} for frame retrieval", layerIndex);
		return PH_ERROR_OUT_OF_RANGE;
	}

	Engine*      engine = ApiDatabase::getResource<Engine>(sessionId);
	HdrRgbFrame* frame  = ApiDatabase::getResource<HdrRgbFrame>(frameId);
	if(!engine || !frame)
	{
		return PH_ERROR_NOT_FOUND;
	}

	engine->retrieveFrame(layerIndex, *frame);
	return PH_OK;
}

PhResult phRetrieveFrameRaw(
	const PhUInt64 sessionId,
	const PhInt32 layerIndex,
	const PhUInt64 frameId)
{
	if(layerIndex < 0)
	{
		PH_LOG(CAPI, Warning, "invalid layer index {} for raw frame retrieval", layerIndex);
		return PH_ERROR_OUT_OF_RANGE;
	}

	Engine*      engine = ApiDatabase::getResource<Engine>(sessionId);
	HdrRgbFrame* frame  = ApiDatabase::getResource<HdrRgbFrame>(frameId);
	if(!engine || !frame)
	{
		return PH_ERROR_NOT_FOUND;
	}

	engine->retrieveFrame(layerIndex, *frame, false);
	return PH_OK;
}

void phGetRenderDimension(const PhUInt64 sessionId, PhUInt32* const out_widthPx, PhUInt32* const out_heightPx)
{
	PH_ASSERT(out_widthPx);
	PH_ASSERT(out_heightPx);
	*out_widthPx = 0;
	*out_heightPx = 0;

	Engine* engine = ApiDatabase::getResource<Engine>(sessionId);
	if(!engine)
	{
		return;
	}
	
	const math::TVector2<int64> dim = engine->getFilmDimensionPx();
	*out_widthPx  = static_cast<PhUInt32>(dim.x());
	*out_heightPx = static_cast<PhUInt32>(dim.y());
}

void phGetRenderObservationInfo(
	const PhUInt64 sessionId,
	PhRenderObservationInfo* const out_info)
{
	PH_ASSERT(out_info);
	out_info->numLayers = 0;
	out_info->numIntegerStats = 0;
	out_info->numRealStats = 0;

	RenderObservationInfo info;
	if(!get_render_observation_info(sessionId, &info))
	{
		return;
	}

	out_info->numLayers = static_cast<PhSize>(info.numLayers());
	out_info->numIntegerStats = static_cast<PhSize>(info.numIntegerStats());
	out_info->numRealStats = static_cast<PhSize>(info.numRealStats());
}

void phGetRenderLayerName(
	const PhUInt64 sessionId,
	PhInt32 layerIndex,
	PhChar* const out_name,
	PhSize* const out_nameLength)
{
	if(layerIndex < 0)
	{
		layerIndex = 0;
	}

	std::string layerName;
	RenderObservationInfo info;
	if(get_render_observation_info(sessionId, &info) && layerIndex < info.numLayers())
	{
		layerName = info.getLayerName(layerIndex);
	}

	// User's responsibility to ensure enough size
	copy_name_to_buffer(layerName, out_name, out_nameLength);
}

void phGetRenderIntegerStatName(
	const PhUInt64 sessionId,
	PhInt32 statIndex,
	PhChar* const out_name,
	PhSize* const out_nameLength)
{
	PH_ASSERT_GE(statIndex, 0);

	std::string statName;
	RenderObservationInfo info;
	if(get_render_observation_info(sessionId, &info) && statIndex < info.numIntegerStats())
	{
		statName = info.getIntegerStatName(statIndex);
	}

	// User's responsibility to ensure enough size
	copy_name_to_buffer(statName, out_name, out_nameLength);
}

void phGetRenderRealStatName(
	const PhUInt64 sessionId,
	PhInt32 statIndex,
	PhChar* const out_name,
	PhSize* const out_nameLength)
{
	PH_ASSERT_GE(statIndex, 0);
	
	std::string statName;
	RenderObservationInfo info;
	if(get_render_observation_info(sessionId, &info) && statIndex < info.numRealStats())
	{
		statName = info.getRealStatName(statIndex);
	}

	// User's responsibility to ensure enough size
	copy_name_to_buffer(statName, out_name, out_nameLength);
}

void phCreateFrame(
	PhUInt64* const out_frameId,
	const PhUInt32 widthPx,
	const PhUInt32 heightPx)
{
	auto frame = std::make_unique<HdrRgbFrame>(widthPx, heightPx);
	*out_frameId = ApiDatabase::addResource(std::move(frame));

	PH_LOG(CAPI, Note, "frame<{}> created", *out_frameId);
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
		PH_LOG(CAPI, Note, "frame<{}> deleted", frameId);
	}
	else
	{
		PH_LOG(CAPI, Warning, "error while deleting frame<{}>", frameId);
	}
}

PhResult phLoadFrame(PhUInt64 frameId, const PhChar* const filePath)
{
	if(!filePath)
	{
		return PH_ERROR_INVALID_ARGUMENT;
	}

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
	return PH_ERROR_OPERATION_FAILED;
}

PhResult phSaveFrame(
	const PhUInt64 frameId, 
	const PhChar* filePath,
	const PhFrameSaveInfo* saveInfo)
{
	if(!filePath)
	{
		return PH_ERROR_INVALID_ARGUMENT;
	}

	const HdrRgbFrame* frame = ApiDatabase::getResource<HdrRgbFrame>(frameId);
	if(!frame)
	{
		return PH_ERROR_NOT_FOUND;
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

		return PH_OK;
	}
	catch(const FileIOError& e)
	{
		PH_LOG(CAPI, Warning,
			"frame<{}> saving failed: {}", frameId, e.whatStr());
	}

	return PH_ERROR_OPERATION_FAILED;
}

PhResult phSaveFrameToBuffer(
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
		return PH_ERROR_NOT_FOUND;
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
			
			return PH_OK;
		}
		catch(const Exception& e)
		{
			PH_LOG(CAPI, Error, "frame<{}> saving failed: {}", frameId, e.what());
		}
	}
	else if(format == PH_BUFFER_FORMAT_FLOAT32_ARRAY)
	{
		// TODO: able to add empty channels (e.g., alpha)

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

		return PH_OK;
	}
	else
	{
		PH_LOG(CAPI, Error,
			"cannot save frame<{}> in unknown format {}", frameId, enum_to_value(format));
		return PH_ERROR_OUT_OF_RANGE;
	}

	return PH_ERROR_OPERATION_FAILED;
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
		PH_LOG(CAPI, Warning, "phFrameOpMSE(2) returned 0 due to invalid frame");
	}

	return MSE;
}

void phAsyncGetRenderProgress(PhUInt64 sessionId, PhRenderProgress* out_progress)
{
	PH_ASSERT(out_progress);
	out_progress->totalWork = 0;
	out_progress->workDone = 0;
	out_progress->elapsedMs = 0;

	auto engine = ApiDatabase::useResource<Engine>(sessionId).lock();
	if(engine && engine->getRenderer())
	{
		RenderProgress progress = engine->getRenderer()->asyncQueryRenderProgress();
		out_progress->totalWork = progress.getTotalWork();
		out_progress->workDone = progress.getWorkDone();
		out_progress->elapsedMs = progress.getElapsedMs();
	}
}

void phAsyncGetRenderStatistics(
	const PhUInt64 sessionId,
	PhFloat32* const out_percentageProgress,
	PhFloat32* const out_samplesPerSecond)
{
	PH_ASSERT(out_percentageProgress);
	PH_ASSERT(out_samplesPerSecond);
	*out_percentageProgress = 0;
	*out_samplesPerSecond = 0;

	auto engine = ApiDatabase::useResource<Engine>(sessionId).lock();
	if(engine)
	{
		float32 percentageProgress, samplesPerSecond;
		engine->asyncQueryStatistics(&percentageProgress, &samplesPerSecond);

		*out_percentageProgress = static_cast<PhFloat32>(percentageProgress);
		*out_samplesPerSecond = static_cast<PhFloat32>(samplesPerSecond);
	}
}

void phAsyncPollUpdatedFrameRegion(
	PhUInt64 sessionId,
	PhFrameRegionInfo* out_regionInfo)
{
	PH_PROFILE_SCOPE();

	PH_ASSERT(out_regionInfo);
	out_regionInfo->xPx = 0;
	out_regionInfo->yPx = 0;
	out_regionInfo->widthPx = 0;
	out_regionInfo->heightPx = 0;
	out_regionInfo->status = PH_FRAME_REGION_STATUS_INVALID;

	auto engine = ApiDatabase::useResource<Engine>(sessionId).lock();
	if(!engine || !engine->getRenderer())
	{
		return;
	}

	RenderRegionStatus region;
	const auto numRegions = engine->getRenderer()->asyncPollUpdatedRegions(
		TSpan<RenderRegionStatus>{&region, 1});
	if(numRegions == 0)
	{
		return;
	}

	to_frame_region_info(region, out_regionInfo);
}

PhSize phAsyncPollUpdatedFrameRegions(
	PhUInt64 sessionId,
	PhUInt64 bufferId,
	PhFrameRegionInfo* out_regionInfos,
	PhSize regionInfoSize)
{
	PH_PROFILE_SCOPE();
	PH_ASSERT(out_regionInfos);

	auto engine = ApiDatabase::useResource<Engine>(sessionId).lock();
	auto buffer = ApiDatabase::useResource<ByteBuffer>(bufferId).lock();
	if(!engine || !engine->getRenderer() || !buffer)
	{
		return 0;
	}

	buffer->rewindWrite();
	TSpan<RenderRegionStatus> regions = make_array_from_buffer<RenderRegionStatus>(
		regionInfoSize, *buffer, true);

	const auto numPolledRegions = engine->getRenderer()->asyncPollUpdatedRegions(regions);

	// Translate from `RenderRegionStatus` to `PhFrameRegionInfo`
	for(std::size_t ri = 0; ri < numPolledRegions; ++ri)
	{
		to_frame_region_info(regions[ri], out_regionInfos + ri);
	}

	return numPolledRegions;
}

PhSize phAsyncPollMergedUpdatedFrameRegions(
	PhUInt64 sessionId,
	PhUInt64 bufferId,
	PhSize mergeSize,
	PhFrameRegionInfo* out_regionInfos,
	PhSize regionInfoSize)
{
	PH_PROFILE_SCOPE();

	PH_ASSERT(out_regionInfos);
	PH_ASSERT_GE(regionInfoSize, 1);

	auto engine = ApiDatabase::useResource<Engine>(sessionId).lock();
	auto buffer = ApiDatabase::useResource<ByteBuffer>(bufferId).lock();
	if(!engine || !engine->getRenderer() || !buffer)
	{
		return 0;
	}

	buffer->rewindWrite();
	TSpan<RenderRegionStatus> regions = make_array_from_buffer<RenderRegionStatus>(
		regionInfoSize, *buffer, true);

	const auto numPolledRegions = engine->getRenderer()->asyncPollMergedUpdatedRegions(regions, mergeSize);

	// Translate from `RenderRegionStatus` to `PhFrameRegionInfo`
	for(std::size_t ri = 0; ri < numPolledRegions; ++ri)
	{
		to_frame_region_info(regions[ri], out_regionInfos + ri);
	}

	return numPolledRegions;
}

void phAsyncPeekFrame(
	PhUInt64 sessionId,
	PhInt32 layerIndex,
	PhUInt32 xPx,
	PhUInt32 yPx,
	PhUInt32 widthPx,
	PhUInt32 heightPx,
	PhUInt64 frameId)
{
	auto engine = ApiDatabase::useResource<Engine>(sessionId).lock();
	auto frame  = ApiDatabase::useResource<HdrRgbFrame>(frameId).lock();
	if(engine && frame)
	{
		Region region({xPx, yPx}, {xPx + widthPx, yPx + heightPx});
		engine->asyncPeekFrame(layerIndex, region, *frame);
	}
}

void phAsyncPeekFrameRaw(
	PhUInt64 sessionId,
	PhInt32 layerIndex,
	PhUInt32 xPx,
	PhUInt32 yPx,
	PhUInt32 widthPx,
	PhUInt32 heightPx,
	PhUInt64 frameId)
{
	auto engine = ApiDatabase::useResource<Engine>(sessionId).lock();
	auto frame  = ApiDatabase::useResource<HdrRgbFrame>(frameId).lock();
	if(engine && frame)
	{
		Region region({xPx, yPx}, {xPx + widthPx, yPx + heightPx});
		engine->asyncPeekFrame(layerIndex, region, *frame, false);
	}
}

void phSetWorkingDirectory(const PhUInt64 sessionId, const PhChar* const workingDirectory)
{
	static_assert(sizeof(PhChar) == sizeof(char));

	Engine* engine = ApiDatabase::getResource<Engine>(sessionId);
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

	PH_LOG(CAPI, Note, "buffer<{}> created", *out_bufferId);
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
		PH_LOG(CAPI, Note, "buffer<{}> deleted", bufferId);
	}
	else
	{
		PH_LOG(CAPI, Warning, "error while deleting buffer<{}>", bufferId);
	}
}
