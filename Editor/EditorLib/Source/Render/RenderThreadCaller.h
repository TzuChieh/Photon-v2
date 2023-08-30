#pragma once

#include "Render/RenderThread.h"

#include <utility>

namespace ph::editor
{

class RenderData;

/*! @brief Thin wrapper around RenderThread specifically for adding render works.
Mainly to hide unrelated interface in `RenderThread` except for work adding. Can only be used between
`RenderThread::beginFrame()` and `RenderThread::endFrame()`.
*/
class RenderThreadCaller final
{
public:
	explicit RenderThreadCaller(RenderThread& renderThread);

	/*!
	Work signature: callable as `void(RenderData&)`, can be wrapped as a `TFunction`.
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
