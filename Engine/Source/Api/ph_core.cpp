#include "ph_core.h"
#include "Api/ApiDatabase.h"
#include "Core/MtImportanceRenderer.h"
#include "Core/Renderer/PreviewRenderer.h"
#include "World/World.h"
#include "Camera/Camera.h"
#include "Camera/DefaultCamera.h"
#include "Core/SampleGenerator/PixelJitterSampleGenerator.h"
#include "Api/test_scene.h"
#include "Image/Film/Film.h"
#include "Image/Frame.h"

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

void phCreateRenderer(PHuint64* out_rendererId, const PHint32 rendererType, const PHuint32 numThreads)
{
	using namespace ph;

	switch(rendererType)
	{
	case PH_PREVIEW_RENDERER_TYPE:
		*out_rendererId = static_cast<std::size_t>(ApiDatabase::addRenderer(std::make_unique<PreviewRenderer>(numThreads)));
		break;

	case PH_IMPORTANCE_RENDERER_TYPE:
		//*out_rendererId = static_cast<std::size_t>(ApiDatabase::addRenderer(std::make_unique<ImportanceRenderer>()));
		std::cerr << "warning: phCreateRenderer(), Photon does not support ImportanceRenderer anymore" << std::endl;
		break;

	case PH_MT_IMPORTANCE_RENDERER_TYPE:
		*out_rendererId = static_cast<std::size_t>(ApiDatabase::addRenderer(std::make_unique<MtImportanceRenderer>(numThreads)));
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
	*out_worldId = static_cast<PHuint64>(ph::ApiDatabase::addWorld(std::make_unique<ph::World>()));
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
		*out_cameraId = static_cast<PHuint64>(ApiDatabase::addCamera(std::make_unique<DefaultCamera>()));
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

void phSetCameraFilm(const PHuint64 cameraId, const PHuint64 filmId)
{
	using namespace ph;

	Camera* camera = ApiDatabase::getCamera(cameraId);
	Film*   film   = ApiDatabase::getFilm(filmId);
	if(camera && film)
	{
		camera->setFilm(film);
	}
}

void phCreateFilm(PHuint64* out_filmId, const PHuint32 filmWidthPx, const PHuint32 filmHeightPx)
{
	using namespace ph;

	*out_filmId = static_cast<PHuint64>(ApiDatabase::addFilm(std::make_unique<Film>(static_cast<uint32>(filmWidthPx), static_cast<uint32>(filmHeightPx))));
}

void phDeleteFilm(const PHuint64 filmId)
{
	if(!ph::ApiDatabase::removeFilm(filmId))
	{
		std::cerr << "error while deleting film<" << filmId << ">" << std::endl;
	}
}

void phCreateSampleGenerator(PHuint64* out_sampleGeneratorId, const PHint32 sampleGeneratorType, const PHuint32 sppBudget)
{
	using namespace ph;

	switch(sampleGeneratorType)
	{
	case PH_PIXEL_JITTER_SAMPLE_GENERATOR_TYPE:
		*out_sampleGeneratorId = static_cast<PHuint64>(ApiDatabase::addSampleGenerator(std::make_unique<PixelJitterSampleGenerator>(sppBudget)));
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

void phLoadTestScene(const PHuint64 worldId)
{
	using namespace ph;

	World* world = ApiDatabase::getWorld(worldId);
	if(world)
	{
		//load5bScene(world);
		loadTestScene(world);
	}
}

void phRender(const PHuint64 rendererId, const PHuint64 worldId, const PHuint64 cameraId)
{
	using namespace ph;

	Renderer* renderer = ApiDatabase::getRenderer(rendererId);
	World*    world    = ApiDatabase::getWorld(worldId);
	Camera*   camera   = ApiDatabase::getCamera(cameraId);
	if(renderer && world && camera)
	{
		if(!camera->getFilm())
		{
			std::cerr << "warning: at phRender(), camera has no film" << std::endl;
			return;
		}

		if(!renderer->isReady())
		{
			std::cerr << "warning: at phRender(), renderer is not ready" << std::endl;
			return;
		}

		renderer->render(*world, *camera);
	}
}

void phSetRendererSampleGenerator(const PHuint64 rendererId, const PHuint64 sampleGeneratorId)
{
	using namespace ph;

	Renderer*        renderer        = ApiDatabase::getRenderer(rendererId);
	SampleGenerator* sampleGenerator = ApiDatabase::getSampleGenerator(sampleGeneratorId);

	if(renderer && sampleGenerator)
	{
		renderer->setSampleGenerator(sampleGenerator);
	}
}

void phCookWorld(const PHuint64 worldId)
{
	using namespace ph;

	World* world = ApiDatabase::getWorld(worldId);
	if(world)
	{
		// HACK
		world->update(0.0f);
	}
}

void phDevelopFilm(const PHuint64 filmId, const PHuint64 frameId)
{
	using namespace ph;

	Film*  film  = ApiDatabase::getFilm(filmId);
	Frame* frame = ApiDatabase::getFrame(frameId);
	if(film && frame)
	{
		film->developFilm(frame);
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

void phSetCameraPosition(const PHuint64 cameraId, const PHfloat32 x, const PHfloat32 y, const PHfloat32 z)
{
	using namespace ph;

	Camera* camera = ApiDatabase::getCamera(cameraId);
	if(camera)
	{
		camera->setPosition(Vector3f(x, y, z));
	}
}

void phSetCameraDirection(const PHuint64 cameraId, const PHfloat32 x, const PHfloat32 y, const PHfloat32 z)
{
	using namespace ph;

	Camera* camera = ApiDatabase::getCamera(cameraId);
	if(camera)
	{
		camera->setDirection(Vector3f(x, y, z).normalizeLocal());
	}
}