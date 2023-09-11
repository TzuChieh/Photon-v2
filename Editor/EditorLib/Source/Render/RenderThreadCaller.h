#pragma once

#include "Render/RenderThread.h"

#include <utility>

namespace ph::editor::render { class System; }

namespace ph::editor
{

/*! @brief Thin `RenderThread` wrapper for interactions from another thread.
Mainly to hide unrelated interface in `RenderThread` except for commonly used, cross-thread operations.
*/
class RenderThreadCaller final
{
public:
	explicit RenderThreadCaller(RenderThread& renderThread);

	/*!
	Work signature: callable as `void(render::System&)`, can be wrapped as a `TFunction`.
	Can only be called between `RenderThread::beginFrame()` and `RenderThread::endFrame()`.
	*/
	template<typename WorkType>
	inline void add(WorkType&& work)
	{
		m_renderThread.addWork(std::forward<WorkType>(work));
	}

private:
	RenderThread& m_renderThread;
};

inline RenderThreadCaller::RenderThreadCaller(RenderThread& renderThread)
	: m_renderThread(renderThread)
{}

}// end namespace ph::editor
