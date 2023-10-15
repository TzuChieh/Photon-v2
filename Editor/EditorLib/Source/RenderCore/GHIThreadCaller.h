#pragma once

#include "RenderCore/GHIThread.h"

#include <utility>

namespace ph::editor::ghi { class GraphicsContext; }

namespace ph::editor
{

/*! @brief Thin wrapper for GHI thread interactions from another thread.
Mainly to hide unrelated GHI interface except for commonly used, cross-thread operations.
*/
class GHIThreadCaller final
{
public:
	GHIThreadCaller(GHIThread& ghiThread, ghi::GraphicsContext& graphicsCtx);

	/*!
	Work signature: callable as `void(ghi::GraphicsContext&)`, can be wrapped as a `TFunction`.
	Can only be used between `GHIThread::beginFrame()` and `GHIThread::endFrame()`.
	*/
	template<typename WorkType>
	void add(WorkType&& work)
	{
		m_ghiThread.addWork(std::forward<WorkType>(work));
	}

	ghi::GraphicsContext& getGraphicsContext();

private:
	GHIThread& m_ghiThread;
	ghi::GraphicsContext& m_graphicsCtx;
};

inline GHIThreadCaller::GHIThreadCaller(GHIThread& ghiThread, ghi::GraphicsContext& graphicsCtx)
	: m_ghiThread(ghiThread)
	, m_graphicsCtx(graphicsCtx)
{}

inline ghi::GraphicsContext& GHIThreadCaller::getGraphicsContext()
{
	return m_graphicsCtx;
}

}// end namespace ph::editor
