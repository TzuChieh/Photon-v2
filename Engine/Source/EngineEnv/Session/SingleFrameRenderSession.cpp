#include "EngineEnv/Session/SingleFrameRenderSession.h"
#include "EngineEnv/CoreCookingContext.h"
#include "Common/Logger.h"
#include "DataIO/SDL/SceneDescription.h"
#include "EngineEnv/Observer/Observer.h"
#include "EngineEnv/SampleSource/SampleSource.h"
#include "EngineEnv/Visualizer/Visualizer.h"

namespace ph
{

namespace
{

const Logger logger(LogSender("Single-Frame Render Session"));

}

void SingleFrameRenderSession::applyToContext(CoreCookingContext& ctx) const
{
	RenderSession::applyToContext(ctx);

	if(m_frameSizePx.product() > 0)
	{
		ctx.setFrameSizePx(m_frameSizePx);
	}
	else
	{
		logger.log(ELogLevel::WARNING_MED,
			"invalid frame size provided: " + m_frameSizePx.toString());
	}
}

std::vector<std::shared_ptr<ICoreSdlResource>> SingleFrameRenderSession::gatherResources(const SceneDescription& scene) const
{
	// TODO: try harder to get resource if not found by name (e.g., check if there is only a single
	// observer, if so, get it)

	std::vector<std::shared_ptr<ICoreSdlResource>> resources = RenderSession::gatherResources(scene);
	resources.reserve(resources.size() + 3);

	auto observer = scene.getResource<Observer>(getObserverName());
	if(observer)
	{
		resources.push_back(std::move(observer));
	}
	else
	{
		logger.log(ELogLevel::WARNING_MED,
			"observer <" + getObserverName() + "> not found");
	}

	auto sampleSource = scene.getResource<SampleSource>(getSampleSourceName());
	if(sampleSource)
	{
		resources.push_back(std::move(sampleSource));
	}
	else
	{
		logger.log(ELogLevel::WARNING_MED,
			"sample source <" + getSampleSourceName() + "> not found");
	}

	auto visualizer = scene.getResource<Visualizer>(getVisualizerName());
	if(visualizer)
	{
		resources.push_back(std::move(visualizer));
	}
	else
	{
		logger.log(ELogLevel::WARNING_MED,
			"visualizer <" + getVisualizerName() + "> not found");
	}

	return std::move(resources);
}

}// end namespace ph
