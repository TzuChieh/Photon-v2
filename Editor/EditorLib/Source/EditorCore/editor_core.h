#pragma once

#include "EditorCore/Thread/Threads.h"
#include "EditorCore/Thread/RenderThread.h"
#include "EditorCore/Thread/GHIThread.h"

#include <utility>

namespace ph::editor
{

template<typename WorkType>
inline void add_render_command(WorkType&& work)
{
	Threads::getRenderThread().addWork(std::forward<WorkType>(work));
}

//template<typename Func>
//inline void add_GHI_command()
//{
//	// TODO
//}

}// end namespace ph::editor
