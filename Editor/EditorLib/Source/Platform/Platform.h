#pragma once

#include <Utility/INoCopyAndMove.h>
#include <Common/primitive_type.h>
#include <Utility/exception.h>

namespace ph::editor
{

class PlatformInput;
class PlatformDisplay;

class PlatformException : public RuntimeException
{
public:
	using RuntimeException::RuntimeException;
};

/*! @brief Platform abstraction.
A editor platform abstraction layer that handles update and render operations.
@exception PlatformException When error occurred and the platform must terminate its operations.
@note This is a RAII resource.
*/
class Platform : private INoCopyAndMove
{
public:
	Platform();
	virtual ~Platform();

	virtual void update(float64 deltaS) = 0;
	virtual void render() = 0;
	virtual const PlatformInput& getInput() const = 0;
	virtual const PlatformDisplay& getDisplay() const = 0;
	virtual bool isInitialized() const = 0;
};

}// end namespace ph::editor
