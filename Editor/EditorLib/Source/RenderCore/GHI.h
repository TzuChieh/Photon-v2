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

	/*! @brief Load and initiate the GHI.
	load() and unload() must be called on the same thread. All remaining methods must be 
	called between calls to load() and unload() (except for ctor and dtor).
	*/
	virtual void load() = 0;

	/*! @brief Unload and clean up the GHI.
	Must be called on the same thread as load(). See load() for more information on the 
	requirements of this method.
	*/
	virtual void unload() = 0;

	virtual void swapBuffers() = 0;

private:
};

}// end namespace ph::editor
