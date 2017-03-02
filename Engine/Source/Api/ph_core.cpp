#include "ph_core.h"
#include "Api/ApiDatabase.h"
#include "Core/Renderer.h"
#include "FileIO/Description.h"
#include "FileIO/DescriptionFileParser.h"
#include "Core/Camera/Camera.h"
#include "Api/test_scene.h"
#include "Core/Camera/Film.h"
#include "PostProcess/Frame.h"
#include "PostProcess/HdrFrame.h"
#include "Math/TArithmeticArray.tpp"

#include <memory>
#include <iostream>

PHint32 phStart()
{
	/*std::array<ph::real, 32> inputs;
	ph::TArithmeticArray<ph::real, 32> aaa(inputs);*/

	return PH_TRUE;
}

void phExit()
{
	ph::ApiDatabase::releaseAllData();
}

void phCreateRenderer(PHuint64* out_rendererId, const PHuint32 numThreads)
{
	using namespace ph;

	*out_rendererId = static_cast<std::size_t>(ApiDatabase::addRenderer(std::make_unique<Renderer>(numThreads)));
}

void phDeleteRenderer(const PHuint64 rendererId)
{
	if(ph::ApiDatabase::removeRenderer(rendererId))
	{
		std::cout << "Renderer<" << rendererId << "> deleted" << std::endl;
	}
	else
	{
		std::cout << "error while deleting Renderer<" << rendererId << ">" << std::endl;
	}
}

void phCreateDescription(PHuint64* out_descriptionId)
{
	*out_descriptionId = static_cast<PHuint64>(ph::ApiDatabase::addDescription(std::make_unique<ph::Description>()));
}

void phLoadDescription(const PHuint64 descriptionId, const char* const filename)
{
	using namespace ph;

	Description* description = ApiDatabase::getDescription(descriptionId);
	if(description)
	{
		DescriptionFileParser parser;
		if(!parser.load(filename, description))
		{
			std::cout << "error while loading file <" << filename << ">" << 
			             " into description <" << descriptionId << ">" << std::endl;
		}
	}
}

void phUpdateDescription(const PHuint64 descriptionId)
{
	using namespace ph;

	Description* description = ApiDatabase::getDescription(descriptionId);
	if(description)
	{
		// HACK
		description->update(0.0f);
	}
}

void phDeleteDescription(const PHuint64 descriptionId)
{
	if(!ph::ApiDatabase::removeDescription(descriptionId))
	{
		std::cerr << "error while deleting Description<" << descriptionId << ">" << std::endl;
	}
}

void phRender(const PHuint64 rendererId, const PHuint64 descriptionId)
{
	using namespace ph;

	Renderer*    renderer = ApiDatabase::getRenderer(rendererId);
	Description* description = ApiDatabase::getDescription(descriptionId);
	if(renderer && description)
	{
		renderer->render(*description);
	}
}

void phDevelopFilm(const PHuint64 descriptionId, const PHuint64 frameId)
{
	using namespace ph;

	Description* description = ApiDatabase::getDescription(descriptionId);
	Frame*       frame       = ApiDatabase::getFrame(frameId);
	if(description && frame)
	{
		description->film->developFilm(frame);
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
		*out_nPixelComponents = frame->nPixelComponents();
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