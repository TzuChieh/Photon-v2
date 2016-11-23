#include "ph_core.h"
#include "Api/ApiDatabase.h"
#include "Core/BruteForceRenderer.h"
#include "Core/ImportanceRenderer.h"
#include "World/World.h"
#include "Camera/DefaultCamera.h"
#include "Core/StandardSampleGenerator.h"

#include <memory>
#include <iostream>

PHint32 phStart()
{
	return PH_TRUE;
}

void phExit()
{
	ph::ApiDatabase::releaseAllData();
}

void phCreateRenderer(PHuint64* out_rendererId, const PHint32 rendererType)
{
	using namespace ph;

	switch(rendererType)
	{
	case PH_BRUTE_FORCE_RENDERER_TYPE:
		*out_rendererId = static_cast<std::size_t>(ApiDatabase::addRenderer(std::make_unique<BruteForceRenderer>()));
		break;

	case PH_IMPORTANCE_RENDERER_TYPE:
		*out_rendererId = static_cast<std::size_t>(ApiDatabase::addRenderer(std::make_unique<ImportanceRenderer>()));
		break;

	default:
		std::cerr << "unidentified renderer type at phCreateRenderer()" << std::endl;
	}
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

void phCreateWorld(PHuint64* out_worldId)
{
	*out_worldId = static_cast<std::size_t>(ph::ApiDatabase::addWorld(std::make_unique<ph::World>()));
}

void phDeleteWorld(const PHuint64 worldId)
{
	if(!ph::ApiDatabase::removeWorld(worldId))
	{
		std::cerr << "error while deleting World<" << worldId << ">" << std::endl;
	}
}

void phCreateCamera(PHuint64* out_cameraId, const PHint32 cameraType)
{
	using namespace ph;

	switch(cameraType)
	{
	case PH_DEFAULT_CAMERA_TYPE:
		*out_cameraId = static_cast<std::size_t>(ApiDatabase::addCamera(std::make_unique<DefaultCamera>()));
		break;

	default:
		std::cerr << "unidentified Camera type at phCreateCamera()" << std::endl;
	}
}

void phDeleteCamera(const PHuint64 cameraId)
{
	if(!ph::ApiDatabase::removeCamera(cameraId))
	{
		std::cerr << "error while deleting Camera<" << cameraId << ">" << std::endl;
	}
}

void phCreateSampleGenerator(PHuint64* out_sampleGeneratorId, const PHint32 sampleGeneratorType, const PHuint32 sppBudget)
{
	using namespace ph;

	switch(sampleGeneratorType)
	{
	case PH_STANDARD_SAMPLE_GENERATOR_TYPE:
		*out_sampleGeneratorId = static_cast<std::size_t>(ApiDatabase::addSampleGenerator(std::make_unique<StandardSampleGenerator>(sppBudget)));
		break;

	default:
		std::cerr << "unidentified SampleGenerator type at phCreateSampleGenerator()" << std::endl;
	}
}

void phDeleteSampleGenerator(const PHuint64 sampleGeneratorId)
{
	if(!ph::ApiDatabase::removeSampleGenerator(sampleGeneratorId))
	{
		std::cerr << "error while deleting SampleGenerator<" << sampleGeneratorId << ">" << std::endl;
	}
}