#include "RenderCore/GraphicsMemoryManager.h"
#include "RenderCore/Memory/GraphicsArena.h"

namespace ph::editor
{

GraphicsMemoryManager::~GraphicsMemoryManager() = default;

GraphicsArena GraphicsMemoryManager::newHostArena()
{
	return GraphicsArena(this, GraphicsArena::EType::Host);
}

GraphicsArena GraphicsMemoryManager::newRenderProducerHostArena()
{
	return GraphicsArena(this, GraphicsArena::EType::RenderProducerHost);
}

}// end namespace ph::editor
