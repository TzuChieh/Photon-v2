#pragma once

#include <Utility/INoCopyAndMove.h>

namespace ph::editor
{

class GHI : private INoCopyAndMove
{
public:
	virtual ~GHI();

	virtual void swapBuffers() = 0;

private:
};

}// end namespace ph::editor
