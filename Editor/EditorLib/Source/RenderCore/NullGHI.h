#pragma once

#include "RenderCore/GHI.h"

namespace ph::editor
{

class NullGHI : public GHI
{
public:
	inline ~NullGHI() override = default;

	inline void load() override
	{}

	inline void unload() override
	{}

	inline void swapBuffers()
	{}

private:
};

}// end namespace ph::editor
