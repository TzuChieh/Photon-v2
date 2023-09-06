#include "RenderCore/GraphicsMemoryManager.h"
#include "RenderCore/Memory/GraphicsArena.h"

namespace ph::editor
{

GraphicsMemoryManager::~GraphicsMemoryManager() = default;

GraphicsArena GraphicsMemoryManager::getHostArena()
{
	return GraphicsArena(this, GraphicsArena::EType::Host);
}

GraphicsArena GraphicsMemoryManager::getRendererHostArena()
{
	return GraphicsArena(this, GraphicsArena::EType::RendererHost);
}

}// end namespace ph::editor
