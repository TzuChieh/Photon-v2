#pragma once

#include "EditorCore/Thread/RenderThread.h"

#include <utility>

namespace ph::editor
{

class RenderThreadCaller final
{
public:
	explicit RenderThreadCaller(RenderThread& renderThread);

	/*!
	Work signature: callable as `void(RTRScene&)`, can be wrapped as a `TFunction`.
	*/
	template<typename WorkType>
	inline void add(WorkType&& work)
	{
		m_renderThread.addWork(std::forward<WorkType>(work));
	}

private:
	RenderThread& m_renderThread;
};

}// end namespace ph::editor
