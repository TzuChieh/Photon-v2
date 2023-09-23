#pragma once

#include "RenderCore/BasicGraphicsMemoryManager.h"

namespace ph::editor::ghi
{

class OpenglContext;

class OpenglMemoryManager : public BasicGraphicsMemoryManager
{
public:
	explicit OpenglMemoryManager(OpenglContext& ctx);

private:
	OpenglContext& m_ctx;
};

}// end namespace ph::editor::ghi
