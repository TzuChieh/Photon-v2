#pragma once

#include "RenderCore/GHI.h"

namespace ph::editor
{

class NullGHI : public GHI
{
public:
	inline NullGHI()
		: GHI(EGraphicsAPI::Unknown)
	{}

	inline ~NullGHI() override = default;

	inline void load() override
	{}

	inline void unload() override
	{}

	inline void swapBuffers()
	{}

private:
	inline void beginRawCommand() override
	{}

	inline void endRawCommand() override
	{}
};

}// end namespace ph::editor
