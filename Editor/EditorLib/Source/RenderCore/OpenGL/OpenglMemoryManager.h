#pragma once

#include "RenderCore/BasicGraphicsMemoryManager.h"

namespace ph::editor
{

class OpenglContext;

class OpenglMemoryManager : public BasicGraphicsMemoryManager
{
public:
	explicit OpenglMemoryManager(OpenglContext& ctx);

private:
	OpenglContext& m_ctx;
};

}// end namespace ph::editor
