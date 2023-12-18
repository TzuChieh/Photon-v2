#include "Core/Renderer/RenderWork.h"

#include <Common/profiling.h>

namespace ph
{

void RenderWork::work()
{
	PH_PROFILE_SCOPE();

	doWork();
}

}// end namespace ph
