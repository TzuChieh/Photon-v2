#include "RenderCore/OpenGL/OpenglMemoryManager.h"

namespace ph::editor
{

OpenglMemoryManager::OpenglMemoryManager(OpenglContext& ctx)
	: BasicGraphicsMemoryManager()
	, m_ctx(ctx)
{}

}// end namespace ph::editor
