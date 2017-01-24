#pragma once

#include "Common/primitive_type.h"
#include "Camera/Camera.h"
#include "World/World.h"
#include "Filmic/Film.h"
#include "Core/Integrator/Integrator.h"

#include <vector>
#include <memory>
#include <string>

namespace ph
{

class SceneDescription final
{
public:
	std::unique_ptr<Camera>     camera;
	std::unique_ptr<Film>       film;
	std::unique_ptr<Integrator> integrator;

	World world;

	SceneDescription();

	void update(const real deltaS);

private:
	bool m_isReady;
};

}// end namespace ph