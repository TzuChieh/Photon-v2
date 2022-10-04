#pragma once

#include <Utility/INoCopyAndMove.h>

namespace ph::editor
{

/*! @brief Graphics API abstraction.
@exception PlatformException When error occurred and the platform must terminate its operations.
*/
class GHI : private INoCopyAndMove
{
public:
	virtual ~GHI();

	virtual void swapBuffers() = 0;

private:
};

}// end namespace ph::editor
