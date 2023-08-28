#pragma once

#include "RenderCore/GraphicsObjectManager.h"

namespace ph::editor
{

class NullObjectManager : public GraphicsObjectManager
{
public:
	inline ~NullObjectManager() override = default;
};

}// end namespace ph::editor
