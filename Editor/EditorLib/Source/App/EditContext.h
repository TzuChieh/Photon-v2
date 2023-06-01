#pragma once

#include "Designer/designer_fwd.h"

#include <Common/primitive_type.h>

namespace ph::editor
{

class EditContext final
{
public:
	DesignerScene* activeScene = nullptr;
	uint32 isActiveScenePaused : 1 = false;
};

}// end namespace ph::editor
