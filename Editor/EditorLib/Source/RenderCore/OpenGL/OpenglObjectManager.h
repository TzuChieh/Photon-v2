#pragma once

#include "RenderCore/GraphicsObjectManager.h"

namespace ph::editor
{

class OpenglObjectManager : public GraphicsObjectManager
{
public:
	~OpenglObjectManager() override;
};

}// end namespace ph::editor
