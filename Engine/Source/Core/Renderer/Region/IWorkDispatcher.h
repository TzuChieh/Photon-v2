#pragma once

#include "Core/Renderer/Region/WorkUnit.h"

namespace ph
{

class IWorkDispatcher
{
public:
	virtual ~IWorkDispatcher() = default;

	virtual bool dispatch(WorkUnit* out_workUnit) = 0;
};

}// end namespace ph