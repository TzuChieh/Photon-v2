#pragma once

#include "Designer/designer_fwd.h"

namespace ph::editor
{

class EditContext final
{
public:
	DesignerScene* activeScene = nullptr;
};

}// end namespace ph::editor
