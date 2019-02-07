#pragma once

#include "Common/primitive_type.h"
#include "Core/Camera/Camera.h"
#include "World/VisualWorld.h"
#include "Core/Filmic/filmic_fwd.h"
#include "Core/SampleGenerator/SampleGenerator.h"
#include "FileIO/RenderOption.h"
#include "FileIO/SDL/NamedResourceStorage.h"
#include "Core/Renderer/Renderer.h"
#include "World/CookSettings.h"

#include <vector>
#include <memory>
#include <string>

namespace ph
{

class SdlResourcePack final
{
public:
	RenderOption         renderOption;
	NamedResourceStorage resources;

	VisualWorld visualWorld;

	SdlResourcePack();

	void update(real deltaS);

	inline std::shared_ptr<Camera>          getCamera()          const { return m_camera;          };
	inline std::shared_ptr<SampleGenerator> getSampleGenerator() const { return m_sampleGenerator; };
	inline std::shared_ptr<Renderer>        getRenderer()        const { return m_renderer;        };
	inline std::shared_ptr<CookSettings>    getCookSettings()    const { return m_cookSettings;    };

private:
	std::shared_ptr<Camera>          m_camera;
	std::shared_ptr<SampleGenerator> m_sampleGenerator;
	std::shared_ptr<Renderer>        m_renderer;
	std::shared_ptr<CookSettings>    m_cookSettings;
};

}// end namespace ph