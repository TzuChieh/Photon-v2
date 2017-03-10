#pragma once

#include "Common/primitive_type.h"
#include "Core/Camera/Camera.h"
#include "World/VisualWorld.h"
#include "Core/Camera/Film.h"
#include "Core/Integrator/Integrator.h"
#include "Core/SampleGenerator/SampleGenerator.h"
#include "FileIO/RenderOption.h"
#include "FileIO/NamedResourceStorage.h"

#include <vector>
#include <memory>
#include <string>

namespace ph
{

class Description final
{
public:
	std::unique_ptr<Camera>          camera;
	std::unique_ptr<Film>            film;
	std::unique_ptr<Integrator>      integrator;
	std::unique_ptr<SampleGenerator> sampleGenerator;

	RenderOption         renderOption;
	NamedResourceStorage resources;

	VisualWorld visualWorld;

	Description();

	void update(const real deltaS);

private:
	bool m_isReady;
};

}// end namespace ph