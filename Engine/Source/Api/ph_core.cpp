#include "ph_core.h"
#include "Api/ApiDatabase.h"
#include "Core/Engine.h"
#include "FileIO/Description.h"
#include "FileIO/DescriptionParser.h"
#include "Core/Camera/Camera.h"
#include "Api/test_scene.h"
#include "Core/Camera/Film.h"
#include "PostProcess/Frame.h"
#include "PostProcess/HdrFrame.h"
#include "Math/TArithmeticArray.tpp"
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

void phCreateFrame(PHuint64* out_frameId, const PHint32 frameType)
{
	switch(frameType)
	{
	case PH_HDR_FRAME_TYPE:
		*out_frameId = ph::ApiDatabase::addFrame(std::make_unique<ph::HdrFrame>());
		std::cout << "Frame<" << *out_frameId << "> created" << std::endl;
		break;

	default:
		std::cerr << "unidentified renderer type at phCreateFrame()" << std::endl;
	}
}

void phGetFrameData(const PHuint64 frameId, const PHfloat32** out_data, PHuint32* out_widthPx, PHuint32* out_heightPx, PHuint32* out_nPixelComponents)
{
	using namespace ph;

	Frame* frame = ApiDatabase::getFrame(frameId);
	if(frame)
	{
		*out_data             = frame->getPixelData();
		*out_widthPx          = frame->getWidthPx();
		*out_heightPx         = frame->getHeightPx();
		*out_nPixelComponents = frame->numPixelComponents();
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