#include "DataIO/SDL/SdlResourcePack.h"
#include "DataIO/SDL/SdlParser.h"
#include "Core/SampleGenerator/SampleGenerator.h"
#include "Core/Filmic/TSamplingFilm.h"

#include <iostream>

namespace ph
{

SdlResourcePack::SdlResourcePack() :
	visualWorld(), renderOption(), 
	m_receiver(nullptr),
	m_sampleGenerator(nullptr),
	m_renderer(nullptr)
{}

void SdlResourcePack::update(const real deltaS)
{
	const std::string& coreResourceName = SdlParser::CORE_DATA_NAME();

	m_receiver        = resources.getResource<Receiver>       (coreResourceName, DataTreatment::REQUIRED());
	m_sampleGenerator = resources.getResource<SampleGenerator>(coreResourceName, DataTreatment::REQUIRED());
	m_renderer        = resources.getResource<Renderer>       (coreResourceName, DataTreatment::REQUIRED());
	if(!m_receiver || !m_sampleGenerator || !m_renderer)
	{
		std::cerr << "warning: at Description::update(), data incomplete" << std::endl;
		return;
	}

	m_cookSettings = resources.getResource<CookSettings>(coreResourceName);
	if(!m_cookSettings)
	{
		m_cookSettings = std::make_shared<CookSettings>();
	}

	visualWorld.setReceiverPosition(m_receiver->getPosition());
	visualWorld.setCookSettings(m_cookSettings);

	const auto& actors = resources.getActors();
	for(const auto& actor : actors)
	{
		visualWorld.addActor(actor);
	}

	visualWorld.cook();
}

}// end namespace ph
