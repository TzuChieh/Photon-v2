#pragma once

#include "Common/primitive_type.h"
#include "Core/Receiver/Receiver.h"
#include "World/VisualWorld.h"
#include "Core/Filmic/filmic_fwd.h"
#include "Core/SampleGenerator/SampleGenerator.h"
#include "DataIO/RenderOption.h"
#include "DataIO/SDL/NamedResourceStorage.h"
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

	inline std::shared_ptr<Receiver>        getReceiver()        const { return m_receiver;        };
	inline std::shared_ptr<SampleGenerator> getSampleGenerator() const { return m_sampleGenerator; };
	inline std::shared_ptr<Renderer>        getRenderer()        const { return m_renderer;        };
	inline std::shared_ptr<CookSettings>    getCookSettings()    const { return m_cookSettings;    };

private:
	std::shared_ptr<Receiver>        m_receiver;
	std::shared_ptr<SampleGenerator> m_sampleGenerator;
	std::shared_ptr<Renderer>        m_renderer;
	std::shared_ptr<CookSettings>    m_cookSettings;
};

}// end namespace ph
