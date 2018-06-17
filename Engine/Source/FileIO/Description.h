#pragma once

#include "Common/primitive_type.h"
#include "Core/Camera/Camera.h"
#include "World/VisualWorld.h"
#include "Core/Filmic/filmic_fwd.h"
#include "Core/Integrator/Integrator.h"
#include "Core/SampleGenerator/SampleGenerator.h"
#include "FileIO/RenderOption.h"
#include "FileIO/SDL/NamedResourceStorage.h"
#include "Core/Renderer/Renderer.h"

#include <vector>
#include <memory>
#include <string>

namespace ph
{

class Description final
{
public:
	RenderOption         renderOption;
	NamedResourceStorage resources;

	VisualWorld visualWorld;

	Description();

	void update(real deltaS);

	inline std::shared_ptr<Camera>               getCamera()          const { return m_camera;          };
	inline std::shared_ptr<SpectralSamplingFilm> getFilm()            const { return m_film;            };
	inline std::shared_ptr<Integrator>           getIntegrator()      const { return m_integrator;      };
	inline std::shared_ptr<SampleGenerator>      getSampleGenerator() const { return m_sampleGenerator; };
	inline std::shared_ptr<Renderer>             getRenderer() const        { return m_renderer;        };

private:
	std::shared_ptr<Camera>               m_camera;
	std::shared_ptr<SpectralSamplingFilm> m_film;
	std::shared_ptr<Integrator>           m_integrator;
	std::shared_ptr<SampleGenerator>      m_sampleGenerator;
	std::shared_ptr<Renderer>             m_renderer;
};

}// end namespace ph