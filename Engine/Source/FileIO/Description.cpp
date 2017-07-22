#include "FileIO/Description.h"
#include "FileIO/DescriptionParser.h"
#include "Core/SampleGenerator/SampleGenerator.h"

#include <iostream>

namespace ph
{

Description::Description() :
	visualWorld(), renderOption(), 
	m_camera(nullptr), m_film(nullptr), m_integrator(nullptr), m_sampleGenerator(nullptr)
{

}

void Description::update(const real deltaS)
{
	const std::string& resourceName = DescriptionParser::CORE_DATA_NAME();

	m_camera          = resources.getResource<Camera>         (resourceName, DataTreatment::REQUIRED());
	m_film            = resources.getResource<Film>           (resourceName, DataTreatment::REQUIRED());
	m_integrator      = resources.getResource<Integrator>     (resourceName, DataTreatment::REQUIRED());
	m_sampleGenerator = resources.getResource<SampleGenerator>(resourceName, DataTreatment::REQUIRED());
	if(!m_camera || !m_film || !m_integrator || !m_sampleGenerator)
	{
		std::cerr << "warning: at Description::update(), data incomplete" << std::endl;
		return;
	}

	const auto& actors = resources.getActors();
	for(const auto& actor : actors)
	{
		visualWorld.addActor(actor);
	}

	visualWorld.cook();
	m_integrator->update(visualWorld.getScene());
	m_camera->setFilm(m_film);
}

}// end namespace ph