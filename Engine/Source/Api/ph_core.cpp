#include "ph_core.h"
#include "Api/ApiDatabase.h"
#include "Core/Engine.h"
#include "FileIO/Description.h"
#include "FileIO/DescriptionParser.h"
#include "Core/Camera/Camera.h"
#include "Api/test_scene.h"
#include "PostProcess/Frame.h"
#include "Math/TArithmeticArray.h"
#include "Api/init_and_exit.h"

#include <memory>
#include <iostream>

int phInit()
{
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

void phCreateEngine(PHuint64* out_engineId, const PHuint32 numRenderThreads)
{
	using namespace ph;

	auto engine = std::make_unique<Engine>();
	engine->setNumRenderThreads(static_cast<std::size_t>(numRenderThreads));
	*out_engineId = static_cast<std::size_t>(ApiDatabase::addEngine(std::move(engine)));
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

void phEnterCommand(const PHuint64 engineId, const char* const commandFragment)
{
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

void phDevelopFilm(const PHuint64 engineId, const PHuint64 frameId)
{
	using namespace ph;

	Engine* engine = ApiDatabase::getEngine(engineId);
	Frame*  frame  = ApiDatabase::getFrame(frameId);
	if(engine && frame)
	{
		engine->developFilm(frame);
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
	auto frame = std::make_unique<ph::Frame>(widthPx, heightPx);
	*out_frameId = ph::ApiDatabase::addFrame(std::move(frame));

	std::cout << "Frame<" << *out_frameId << "> created" << std::endl;
}

void phGetFrameDimension(const PHuint64 frameId, 
                         PHuint32* const out_widthPx, PHuint32* const out_heightPx)
{
	using namespace ph;

	Frame* frame = ApiDatabase::getFrame(frameId);
	if(frame)
	{
		*out_widthPx  = static_cast<PHuint32>(frame->widthPx());
		*out_heightPx = static_cast<PHuint32>(frame->heightPx());
	}
}

void phGetFrameRgbData(const PHuint64 frameId, const PHfloat32** const out_data)
{
	using namespace ph;

	Frame* frame = ApiDatabase::getFrame(frameId);
	if(frame)
	{
		*out_data = frame->getRgbData();
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

void phQueryRendererPercentageProgress(const PHuint64 engineId, PHfloat32* const out_percentage)
{
	using namespace ph;

	Engine* engine = ApiDatabase::getEngine(engineId);
	if(engine)
	{
		*out_percentage = engine->queryPercentageProgress();
	}
}

void phQueryRendererSampleFrequency(const PHuint64 engineId, PHfloat32* const out_frequency)
{
	using namespace ph;

	Engine* engine = ApiDatabase::getEngine(engineId);
	if(engine)
	{
		*out_frequency = engine->querySampleFrequency();
	}
}