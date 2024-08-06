#include "EngineEnv/Session/SingleFrameRenderSession.h"
#include "EngineEnv/CoreCookingContext.h"
#include "SDL/SceneDescription.h"
#include "EngineEnv/Observer/Observer.h"
#include "EngineEnv/SampleSource/SampleSource.h"
#include "EngineEnv/Visualizer/Visualizer.h"

#include <Common/logging.h>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(SingleFrameRenderSession, RenderSession);

void SingleFrameRenderSession::applyToContext(CoreCookingContext& ctx) const
{
	RenderSession::applyToContext(ctx);

	if(m_frameSizePx.product() > 0)
	{
		ctx.setFrameSizePx(m_frameSizePx);
	}
	else
	{
		PH_LOG(SingleFrameRenderSession, Warning,
			"invalid frame size provided: {}", m_frameSizePx.toString());
	}

	ctx.setTopLevelAcceleratorType(m_topLevelAcceleratorType);
}

std::vector<std::shared_ptr<CoreSdlResource>> SingleFrameRenderSession::gatherResources(const SceneDescription& scene) const
{
	// TODO: try harder to get resource if not found by name (e.g., check if there is only a single
	// observer, if so, get it)

	std::vector<std::shared_ptr<CoreSdlResource>> resources = RenderSession::gatherResources(scene);
	resources.reserve(resources.size() + 3);

	auto observer = scene.getResources().getTyped<Observer>(getObserverName());
	if(observer)
	{
		resources.push_back(std::move(observer));
	}
	else
	{
		PH_LOG(SingleFrameRenderSession, Warning,
			"observer <{}> not found", getObserverName());
	}

	auto sampleSource = scene.getResources().getTyped<SampleSource>(getSampleSourceName());
	if(sampleSource)
	{
		resources.push_back(std::move(sampleSource));
	}
	else
	{
		PH_LOG(SingleFrameRenderSession, Warning,
			"sample source <{}> not found", getSampleSourceName());
	}

	auto visualizer = scene.getResources().getTyped<Visualizer>(getVisualizerName());
	if(visualizer)
	{
		resources.push_back(std::move(visualizer));
	}
	else
	{
		PH_LOG(SingleFrameRenderSession, Warning,
			"visualizer <{}> not found", getVisualizerName());
	}

	return resources;
}

}// end namespace ph
