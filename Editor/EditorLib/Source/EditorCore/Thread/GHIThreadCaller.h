#pragma once

#include "EditorCore/Thread/GHIThread.h"

#include <utility>

namespace ph::editor
{

class GHIThreadCaller final
{
public:
	explicit GHIThreadCaller(GHIThread& ghiThread);

	/*!
	Work signature: callable as `void(GHI&)`, can be wrapped as a `TFunction`.
	*/
	template<typename WorkType>
	inline void add(WorkType&& work)
	{
		m_ghiThread.addWork(std::forward<WorkType>(work));
	}

private:
	GHIThread& m_ghiThread;
};

}// end namespace ph::editor
