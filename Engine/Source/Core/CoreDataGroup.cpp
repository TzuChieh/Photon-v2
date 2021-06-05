#include "Core/CoreDataGroup.h"
#include "DataIO/SDL/SceneDescription.h"
#include "World/CookSettings.h"
#include "Common/Logger.h"
#include "Core/EngineOption.h"
#include "World/VisualWorld.h"
#include "Core/Renderer/Renderer.h"
#include "Core/Receiver/Receiver.h"
#include "Core/SampleGenerator/SampleGenerator.h"

namespace ph
{

namespace
{

Logger logger(LogSender("Core Data"));

}

CoreDataGroup::CoreDataGroup() : 
	m_renderer       (nullptr),
	m_receiver       (nullptr),
	m_sampleGenerator(nullptr),
	m_cookSettings   (nullptr)
{}

bool CoreDataGroup::gatherFromRaw(const SceneDescription& scene)
{
	// Get settings for engine

	const auto engineOptions = scene.getResources<EngineOption>();
	if(engineOptions.empty())
	{
		logger.log(ELogLevel::FATAL_ERROR,
			"no engine option present");
		return false;
	}

	if(engineOptions.size() > 1)
	{
		logger.log(ELogLevel::WARNING_MED,
			"multiple engine options present; picking one of them");
	}
	const auto engineOption = engineOptions.front();

	// Get core resources specified by engine option

	m_renderer = scene.getResource<Renderer>(engineOption->getRendererName(),
		DataTreatment::REQUIRED());

	m_receiver = scene.getResource<Receiver>(engineOption->getReceiverName(),
		DataTreatment::REQUIRED());

	m_sampleGenerator = scene.getResource<SampleGenerator>(engineOption->getSampleGeneratorName(),
		DataTreatment::REQUIRED());

	if(!m_renderer || !m_receiver || !m_sampleGenerator)
	{
		logger.log(ELogLevel::FATAL_ERROR,
			"missing one or more core raw resources");
		return false;
	}

	m_cookSettings = scene.getResource<CookSettings>(engineOption->getCookSettingsName());
	if(!m_cookSettings)
	{
		logger.log(
			"using default cook settings");

		m_cookSettings = std::make_shared<CookSettings>();
	}

	return true;
}

bool CoreDataGroup::gatherFromCooked(const VisualWorld& world)
{
	m_intersector    = world.getIntersector();
	m_emitterSampler = world.getEmitterSampler();
	m_scene          = world.getScene();

	if(!m_intersector || !m_emitterSampler || !m_scene)
	{
		logger.log(ELogLevel::FATAL_ERROR,
			"missing one or more core cooked resources");
		return false;
	}

	return true;
}

}// end namespace ph
