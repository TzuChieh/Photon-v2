#include "EngineEnv/Session/RenderSession.h"
#include "EngineEnv/CoreCookingContext.h"

namespace ph
{

void RenderSession::applyToContext(CoreCookingContext& ctx) const
{
	ctx.setNumWorkers(m_numWorkers);
}

std::vector<std::shared_ptr<CoreSdlResource>> RenderSession::gatherResources(const SceneDescription& scene) const
{
	return {};
}

}// end namespace ph
