#include "FileIO/Description.h"

#include <iostream>

namespace ph
{

Description::Description() :
	camera(nullptr), film(nullptr), integrator(nullptr), sampleGenerator(nullptr), 
	world(), renderOption(), 
	m_isReady(false)
{

}

void Description::update(const real deltaS)
{
	if(!camera || !film || !integrator || !sampleGenerator)
	{
		std::cerr << "warning: at Description::cook(), data incomplete" << std::endl;
		m_isReady = false;
		return;
	}

	world.update(deltaS);

	integrator->update(world);
	sampleGenerator->analyze(world, *film);

	camera->setFilm(film.get());
}

}// end namespace ph