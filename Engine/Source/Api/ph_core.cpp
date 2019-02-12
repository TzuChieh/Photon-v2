#include "ph_core.h"
#include "Api/ApiDatabase.h"
#include "Core/Engine.h"
#include "Core/Camera/Camera.h"
#include "Api/test_scene.h"
#include "Frame/TFrame.h"
#include "Math/TArithmeticArray.h"
#include "Api/init_and_exit.h"
#include "Core/Renderer/Renderer.h"
#include "FileIO/FileSystem/Path.h"
#include "Common/assertion.h"
#include "FileIO/PictureSaver.h"
#include "Api/ApiHelper.h"
#include "Core/Renderer/Region/Region.h"
#include "Common/config.h"
#include "Common/Logger.h"

#include <memory>
#include <iostream>
#include <cstring>

namespace
{
	const ph::Logger logger(ph::LogSender("Core API"));
}

void phConfigCoreResourceDirectory(const PHchar* const directory)
{
	PH_ASSERT(directory);

	ph::Config::CORE_RESOURCE_DIRECTORY() = std::string(directory);
}

int phInit()
{
	ph::init_core_infrastructure();

	if(!ph::init_command_parser())
	{
		logger.log(ph::ELogLevel::FATAL_ERROR, 
			"command parser initialization failed");
		return PH_FALSE;
	}

	return PH_TRUE;
}

int phExit()
{
	if(!ph::exit_api_database())
	{
		logger.log(ph::ELogLevel::FATAL_ERROR,
			"API database exiting failed");
		return PH_FALSE;
	}

	return PH_TRUE;
}

void phCreateEngine(PHuint64* const out_engineId, const PHuint32 numRenderThreads)
{
	PH_ASSERT(out_engineId);

	using namespace ph;

	auto engine = std::make_unique<Engine>();
	engine->setNumRenderThreads(static_cast<std::size_t>(numRenderThreads));
	*out_engineId = static_cast<std::size_t>(ApiDatabase::addEngine(std::move(engine)));

	logger.log("engine<" + std::to_string(*out_engineId) + "> created");
}

void phSetNumRenderThreads(const PHuint64 engineId, const PHuint32 numRenderThreads)
{
	using namespace ph;

	Engine* engine = ApiDatabase::getEngine(engineId);
	if(engine)
	{
		engine->setNumRenderThreads(static_cast<uint32>(numRenderThreads));
	}
}

void phDeleteEngine(const PHuint64 engineId)
{
	if(ph::ApiDatabase::removeEngine(engineId))
	{
		logger.log("engine<" + std::to_string(engineId) + "> deleted");
	}
	else
	{
		logger.log("error while deleting engine<" + std::to_string(engineId) + ">");
	}
}

void phEnterCommand(const PHuint64 engineId, const PHchar* const commandFragment)
{
	static_assert(sizeof(PHchar) == sizeof(char));
	PH_ASSERT(commandFragment);

	using namespace ph;

	Engine* engine = ApiDatabase::getEngine(engineId);
	if(engine)
	{
		engine->enterCommand(std::string(commandFragment));
	}
}

void phRender(const PHuint64 engineId)
{
	using namespace ph;

	Engine* engine = ApiDatabase::getEngine(engineId);
	if(engine)
	{
		engine->render();
	}
}

void phUpdate(const PHuint64 engineId)
{
	using namespace ph;

	Engine* engine = ApiDatabase::getEngine(engineId);
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
	using namespace ph;

	Engine*      engine = ApiDatabase::getEngine(engineId);
	HdrRgbFrame* frame  = ApiDatabase::getFrame(frameId);
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
	using namespace ph;

	Engine*      engine = ApiDatabase::getEngine(engineId);
	HdrRgbFrame* frame  = ApiDatabase::getFrame(frameId);
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
	using namespace ph;

	Engine* engine = ApiDatabase::getEngine(engineId);
	if(engine)
	{
		const TVector2<int64> dim = engine->getFilmDimensionPx();
		*out_widthPx  = static_cast<PHuint32>(dim.x);
		*out_heightPx = static_cast<PHuint32>(dim.y);
	}
}

void phGetObservableRenderData(
	const PHuint64                       engineId,
	struct PHObservableRenderData* const out_data)
{
	PH_ASSERT(out_data);

	using namespace ph;

	Engine* engine = ApiDatabase::getEngine(engineId);
	if(engine)
	{
		const auto data = engine->getRenderer()->getObservableData();

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
			if(i < data.numIntegerStates())
			{
				std::strncpy(
					out_data->integers[i],
					data.getIntegerStateName(i).c_str(),
					PH_MAX_NAME_LENGTH);
				out_data->integers[i][PH_MAX_NAME_LENGTH] = '\0';
			}
		}

		for(std::size_t i = 0; i < PH_NUM_RENDER_STATE_REALS; ++i)
		{
			out_data->reals[i][0] = '\0';
			if(i < data.numRealStates())
			{
				std::strncpy(
					out_data->reals[i],
					data.getRealStateName(i).c_str(),
					PH_MAX_NAME_LENGTH);
				out_data->reals[i][PH_MAX_NAME_LENGTH] = '\0';
			}
		}
	}
}

void phCreateFrame(PHuint64* const out_frameId,
                   const PHuint32 widthPx, const PHuint32 heightPx)
{
	auto frame = std::make_unique<ph::HdrRgbFrame>(widthPx, heightPx);
	*out_frameId = ph::ApiDatabase::addFrame(std::move(frame));

	logger.log("frame<" + std::to_string(*out_frameId) + "> created");
}

void phGetFrameDimension(const PHuint64 frameId, 
                         PHuint32* const out_widthPx, PHuint32* const out_heightPx)
{
	using namespace ph;

	HdrRgbFrame* frame = ApiDatabase::getFrame(frameId);
	if(frame)
	{
		*out_widthPx  = static_cast<PHuint32>(frame->widthPx());
		*out_heightPx = static_cast<PHuint32>(frame->heightPx());
	}
}

void phGetFrameRgbData(const PHuint64 frameId, const PHfloat32** const out_data)
{
	using namespace ph;

	HdrRgbFrame* frame = ApiDatabase::getFrame(frameId);
	if(frame)
	{
		static_assert(sizeof(PHfloat32) == sizeof(HdrComponent));

		*out_data = static_cast<const PHfloat32*>(frame->getPixelData());
	}
}

void phDeleteFrame(const PHuint64 frameId)
{
	if(ph::ApiDatabase::removeFrame(frameId))
	{
		logger.log("frame<" + std::to_string(frameId) + "> deleted");
	}
	else
	{
		logger.log("error while deleting frame<" + std::to_string(frameId) + ">");
	}
}

void phSaveFrame(const PHuint64 frameId, const PHchar* const filePath)
{
	PH_ASSERT(filePath != nullptr);

	using namespace ph;

	const HdrRgbFrame* frame = ApiDatabase::getFrame(frameId);
	if(frame)
	{
		if(!PictureSaver::save(*frame, Path(filePath)))
		{
			logger.log("frame<" + std::to_string(frameId) + "> saving failed");
		}
	}
}

void phAsyncGetRendererStatistics(const PHuint64 engineId,
                                  PHfloat32* const out_percentageProgress,
                                  PHfloat32* const out_samplesPerSecond)
{
	using namespace ph;

	Engine* engine = ApiDatabase::getEngine(engineId);
	if(engine)
	{
		float32 percentageProgress, samplesPerSecond;
		engine->asyncQueryStatistics(&percentageProgress, &samplesPerSecond);

		*out_percentageProgress = static_cast<PHfloat32>(percentageProgress);
		*out_samplesPerSecond   = static_cast<PHfloat32>(samplesPerSecond);
	}
}

void phAsyncGetRendererState(
	const PHuint64               engineId,
	struct PHRenderState* const out_state)
{
	PH_ASSERT(out_state);

	using namespace ph;

	Engine* engine = ApiDatabase::getEngine(engineId);
	if(engine)
	{
		const RenderState state = engine->getRenderer()->asyncQueryRenderState();

		for(std::size_t i = 0; i < PH_NUM_RENDER_STATE_INTEGERS; ++i)
		{
			out_state->integers[i] = static_cast<PHint64>(state.getIntegerState(i));
		}
		for(std::size_t i = 0; i < PH_NUM_RENDER_STATE_REALS; ++i)
		{
			out_state->reals[i] = static_cast<PHfloat32>(state.getRealState(i));
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

	using namespace ph;

	Engine* engine = ApiDatabase::getEngine(engineId);
	if(engine)
	{
		Region region;
		const ERegionStatus status = engine->asyncPollUpdatedRegion(&region);

		*out_xPx      = static_cast<PHuint32>(region.minVertex.x);
		*out_yPx      = static_cast<PHuint32>(region.minVertex.y);
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
	using namespace ph;

	Engine*      engine = ApiDatabase::getEngine(engineId);
	HdrRgbFrame* frame  = ApiDatabase::getFrame(frameId);
	if(engine && frame)
	{
		Region region({xPx, yPx}, {xPx + widthPx, yPx + heightPx});
		engine->asyncPeekFrame(channelIndex, region, *frame);
	}
}

void phSetWorkingDirectory(const PHuint64 engineId, const PHchar* const workingDirectory)
{
	// TODO: static assertion
	PH_ASSERT(sizeof(PHchar) == sizeof(char));

	using namespace ph;

	Engine* engine = ApiDatabase::getEngine(engineId);
	if(engine)
	{
		const Path path(workingDirectory);
		engine->setWorkingDirectory(path);
	}
}