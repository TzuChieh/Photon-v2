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

	inline std::unique_ptr<GHI> clone() override
	{
		return std::make_unique<NullGHI>();
	}

private:
};

}// end namespace ph::editor
