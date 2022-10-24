#pragma once

#include <Utility/INoCopyAndMove.h>

#include <memory>

namespace ph::editor
{

/*! @brief Graphics API abstraction.
@exception PlatformException When error occurred and the platform must terminate its operations.
*/
class GHI : private INoCopyAndMove
{
public:
	virtual ~GHI();

	virtual void load() = 0;
	virtual void unload() = 0;
	virtual void swapBuffers() = 0;
	virtual std::unique_ptr<GHI> clone() = 0;

private:
};

}// end namespace ph::editor
