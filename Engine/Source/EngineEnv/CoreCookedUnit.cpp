#include "EngineEnv/CoreCookedUnit.h"
#include "SDL/SceneDescription.h"
#include "Common/assertion.h"
#include "Common/logging.h"
#include "World/VisualWorld.h"
#include "Core/Renderer/Renderer.h"
#include "Core/Receiver/Receiver.h"
#include "Core/SampleGenerator/SampleGenerator.h"

#include <utility>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(CoreCookedUnit, Core);

CoreCookedUnit::CoreCookedUnit() :
	m_renderer       (nullptr),
	m_receiver       (nullptr),
	m_sampleGenerator(nullptr)
{}

//bool CoreCookedUnit::gatherFromRaw(const SceneDescription& scene)
//{
//	// Get settings for engine
//
//	const auto engineOptions = scene.getResources<EngineOption>();
//	if(engineOptions.empty())
//	{
//		logger.log(ELogLevel::FATAL_ERROR,
//			"no engine option present");
//		return false;
//	}
//
//	if(engineOptions.size() > 1)
//	{
//		logger.log(ELogLevel::WARNING_MED,
//			"multiple engine options present; picking one of them without any rules");
//	}
//	const auto engineOption = engineOptions.front();
//
//	// Get core resources specified by engine option
//
//	m_renderer        = scene.getResource<Renderer>(engineOption->getRendererName());
//	m_receiver        = scene.getResource<Receiver>(engineOption->getReceiverName());
//	m_sampleGenerator = scene.getResource<SampleGenerator>(engineOption->getSampleGeneratorName());
//
//	if(!m_renderer || !m_receiver || !m_sampleGenerator)
//	{
//		logger.log(ELogLevel::FATAL_ERROR,
//			"missing one or more core raw resources");
//		return false;
//	}
//
//	m_cookSettings = scene.getResource<CookSettings>(engineOption->getCookSettingsName());
//	if(!m_cookSettings)
//	{
//		logger.log(
//			"using default cook settings");
//
//		m_cookSettings = std::make_shared<CookSettings>();
//	}
//
//	return true;
//}

//bool CoreCookedUnit::gatherFromCooked(const VisualWorld& world)
//{
//	m_intersector    = world.getIntersector();
//	m_emitterSampler = world.getEmitterSampler();
//	m_scene          = world.getScene();
//
//	if(!m_intersector || !m_emitterSampler || !m_scene)
//	{
//		logger.log(ELogLevel::FATAL_ERROR,
//			"missing one or more core cooked resources");
//		return false;
//	}
//
//	return true;
//}

void CoreCookedUnit::addRenderer(std::unique_ptr<Renderer> renderer)
{
	PH_ASSERT(renderer);

	m_renderer = std::move(renderer);
}

void CoreCookedUnit::addReceiver(std::unique_ptr<Receiver> receiver)
{
	PH_ASSERT(receiver);

	m_receiver = std::move(receiver);
}

void CoreCookedUnit::addSampleGenerator(std::unique_ptr<SampleGenerator> sampleGenerator)
{
	PH_ASSERT(sampleGenerator);

	m_sampleGenerator = std::move(sampleGenerator);
}

}// end namespace ph
