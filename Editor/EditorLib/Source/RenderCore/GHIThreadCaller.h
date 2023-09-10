#pragma once

#include "RenderCore/GHIThread.h"

#include <utility>

namespace ph::editor
{

/*! @brief Thin `GHIThread` wrapper for interactions from another thread.
Mainly to hide unrelated interface in `GHIThread` except for commonly used, cross-thread operations.
*/
class GHIThreadCaller final
{
public:
	explicit GHIThreadCaller(GHIThread& ghiThread);

	/*!
	Work signature: callable as `void(GraphicsContext&)`, can be wrapped as a `TFunction`.
	Can only be used between `GHIThread::beginFrame()` and `GHIThread::endFrame()`.
	*/
	template<typename WorkType>
	inline void add(WorkType&& work)
	{
		m_ghiThread.addWork(std::forward<WorkType>(work));
	}

private:
	GHIThread& m_ghiThread;
};

inline GHIThreadCaller::GHIThreadCaller(GHIThread& ghiThread)
	: m_ghiThread(ghiThread)
{}

}// end namespace ph::editor
