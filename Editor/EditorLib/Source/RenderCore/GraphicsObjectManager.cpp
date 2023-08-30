#include "RenderCore/GraphicsObjectManager.h"
#include "EditorCore/Thread/Threads.h"

#include <Utility/exception.h>

namespace ph::editor
{

GraphicsObjectManager::~GraphicsObjectManager() = default;

void GraphicsObjectManager::setGHIThread(GHIThread* const thread)
{
	if(!Threads::isOnGHIThread())
	{
		throw IllegalOperationException(
			"Attempting to set GHI thread from a non-GHI thread.");
	}

	m_ghiThread = thread;
}

}// end namespace ph::editor
