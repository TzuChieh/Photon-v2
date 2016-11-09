#include "ph_core.h"
#include "Api/ApiDatabase.h"
#include "Core/BruteForceRenderer.h"
#include "Core/ImportanceRenderer.h"

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