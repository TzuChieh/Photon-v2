#include "FileIO/SDL/SdlResourcePack.h"
#include "FileIO/SDL/SdlParser.h"
#include "Core/SampleGenerator/SampleGenerator.h"
#include "Core/Filmic/TSamplingFilm.h"

#include <iostream>

namespace ph
{

SdlResourcePack::SdlResourcePack() :
	visualWorld(), renderOption(), 
	m_camera(nullptr), 
	m_sampleGenerator(nullptr),
	m_renderer(nullptr)
{}

void SdlResourcePack::update(const real deltaS)
{
	const std::string& resourceName = SdlParser::CORE_DATA_NAME();

	m_camera          = resources.getResource<Camera>         (resourceName, DataTreatment::REQUIRED());
	m_sampleGenerator = resources.getResource<SampleGenerator>(resourceName, DataTreatment::REQUIRED());
	m_renderer        = resources.getResource<Renderer>       (resourceName, DataTreatment::REQUIRED());
	if(!m_camera || !m_sampleGenerator || !m_renderer)
	{
		std::cerr << "warning: at Description::update(), data incomplete" << std::endl;
		return;
	}

	visualWorld.setCameraPosition(m_camera->getPosition());

	const auto& actors = resources.getActors();
	for(const auto& actor : actors)
	{
		visualWorld.addActor(actor);
	}

	visualWorld.cook();

	const real aspectRatio = static_cast<real>(m_renderer->getRenderWidthPx()) / m_renderer->getRenderHeightPx();
	m_camera->setAspectRatio(aspectRatio);
}

}// end namespace ph