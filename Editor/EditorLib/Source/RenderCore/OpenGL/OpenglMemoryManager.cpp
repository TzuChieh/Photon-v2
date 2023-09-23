#include "RenderCore/OpenGL/OpenglMemoryManager.h"

namespace ph::editor::ghi
{

OpenglMemoryManager::OpenglMemoryManager(OpenglContext& ctx)
	: BasicGraphicsMemoryManager()
	, m_ctx(ctx)
{}

}// end namespace ph::editor::ghi
