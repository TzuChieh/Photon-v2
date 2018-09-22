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

#include <memory>
#include <iostream>

void phConfigCoreResourceDirectory(const PHchar* const directory)
{
	PH_ASSERT(directory);

	ph::Config::CORE_RESOURCE_DIRECTORY() = ph::Path(directory);
}

int phInit()
{
	ph::init_core_infrastructure();

	if(!ph::init_command_parser())
	{
		std::cerr << "command parser initialization failed" << std::endl;
		return PH_FALSE;
	}

	return PH_TRUE;
}

int phExit()
{
	if(!ph::exit_api_database())
	{
		std::cerr << "API database exiting failed" << std::endl;
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
		std::cout << "Engine<" << engineId << "> deleted" << std::endl;
	}
	else
	{
		std::cerr << "error while deleting Engine<" << engineId << ">" << std::endl;
	}
}

void phEnterCommand(const PHuint64 engineId, const PHchar* const commandFragment)
{
	// TODO: static assertion
	PH_ASSERT(sizeof(PHchar) == sizeof(char));

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

void phDevelopFilm(
	const PHuint64      engineId, 
	const PHuint64      frameId, 
	const PH_EATTRIBUTE attribute)
{
	using namespace ph;

	Engine*      engine = ApiDatabase::getEngine(engineId);
	HdrRgbFrame* frame  = ApiDatabase::getFrame(frameId);
	if(engine && frame)
	{
		engine->developFilm(*frame, ApiHelper::toCppAttribute(attribute));
	}
}

void phDevelopFilmRaw(
	const PHuint64      engineId, 
	const PHuint64      frameId, 
	const PH_EATTRIBUTE attribute)
{
	using namespace ph;

	Engine*      engine = ApiDatabase::getEngine(engineId);
	HdrRgbFrame* frame  = ApiDatabase::getFrame(frameId);
	if(engine && frame)
	{
		engine->developFilm(*frame, ApiHelper::toCppAttribute(attribute), false);
	}
}

void phGetFilmDimension(const PHuint64 engineId, PHuint32* const out_widthPx, PHuint32* const out_heightPx)
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

void phCreateFrame(PHuint64* const out_frameId,
                   const PHuint32 widthPx, const PHuint32 heightPx)
{
	auto frame = std::make_unique<ph::HdrRgbFrame>(widthPx, heightPx);
	*out_frameId = ph::ApiDatabase::addFrame(std::move(frame));

	std::cout << "Frame<" << *out_frameId << "> created" << std::endl;
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
		std::cout << "Frame<" << frameId << "> deleted" << std::endl;
	}
	else
	{
		std::cout << "error while deleting Frame<" << frameId << ">" << std::endl;
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
			std::cerr << "Frame<" << frameId << "> saving failed" << std::endl;
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

int phAsyncPollUpdatedFilmRegion(const PHuint64 engineId,
                                 PHuint32* const out_xPx, PHuint32* const out_yPx,
                                 PHuint32* const out_widthPx, PHuint32* const out_heightPx)
{
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

void phAsyncDevelopFilmRegion(
	const PHuint64      engineId,
	const PHuint64      frameId,
	const PHuint32      xPx, 
	const PHuint32      yPx,
	const PHuint32      widthPx, 
	const PHuint32      heightPx, 
	const PH_EATTRIBUTE attribute)
{
	using namespace ph;

	Engine*      engine = ApiDatabase::getEngine(engineId);
	HdrRgbFrame* frame  = ApiDatabase::getFrame(frameId);
	if(engine && frame)
	{
		Region region({xPx, yPx}, {xPx + widthPx, yPx + heightPx});
		engine->asyncDevelopFilmRegion(*frame, region, ApiHelper::toCppAttribute(attribute));
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