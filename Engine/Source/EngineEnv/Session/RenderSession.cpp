#include "EngineEnv/Session/RenderSession.h"
#include "EngineEnv/CoreCookingContext.h"

namespace ph
{

inline void RenderSession::applyToContext(CoreCookingContext& ctx) const
{
	ctx.setNumWorkers(m_numWorkers);
}

inline std::vector<std::shared_ptr<CoreSdlResource>> RenderSession::gatherResources(const SceneDescription& scene) const
{}

}// end namespace ph
