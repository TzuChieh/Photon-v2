#include "FileIO/SceneDescription.h"

#include <iostream>

namespace ph
{

SceneDescription::SceneDescription() :
	camera(nullptr), film(nullptr), integrator(nullptr), 
	world(), 
	m_isReady(false)
{

}

void SceneDescription::update(const real deltaS)
{
	if(!camera || !film || !integrator)
	{
		std::cerr << "warning: at SceneDescription::cook(), data incomplete" << std::endl;
		m_isReady = false;
		return;
	}

	camera->setFilm(film.get());

	world.update(deltaS);
	integrator->update(world);
}

}// end namespace ph