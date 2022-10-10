#pragma once

#include "EditorCore/EKeyCode.h"
#include "EditorCore/EMouseCode.h"

#include <Utility/INoCopyAndMove.h>
#include <Common/primitive_type.h>
#include <Math/TVector2.h>

namespace ph::editor
{

/*! @brief Platform input abstraction.
@exception PlatformException When error occurred and the platform must terminate its operations.
@note Can only be created by `Platform`.
*/
class PlatformInput : private INoCopyAndMove
{
	friend class Platform;

protected:
	PlatformInput();
	virtual ~PlatformInput();

public:
	/*!
	Update all states for input signals and fire all events afterwards.
	*/
	virtual void poll(float64 deltaS) = 0;

	/*!
	"Virtualize Cursor" means to hide the cursor and gives user a sense of
	unlimited cursor movement. Note that under this mode the absolute position
	of the cursor is unreliable, only relative measurements have their meanings.
	*/
	///@{
	virtual void virtualizeCursor() const = 0;
	virtual void unvirtualizeCursor() const = 0;
	///@}

	/*!
	Returns true if the key/button was detected as pressed after current update; 
	otherwise, false is returned.
	*/
	///@{
	virtual bool isKeyDown(EKeyCode keyCode) const = 0;
	virtual bool isMouseButtonDown(EMouseCode mouseCode) const = 0;
	///@}

	/*!
	Returns true if the key/button was detected as released after current update; 
	otherwise, false is returned.
	*/
	///@{
	virtual bool isKeyUp(EKeyCode keyCode) const = 0;
	virtual bool isMouseButtonUp(EMouseCode mouseCode) const = 0;
	///@}

	/*!
	Returns true if the key/button is continuously being pressed until it starts
	to repeatedly firing signals. Notice that this method will return false while 
	is<X>Down() or is<X>Up() is true.
	*/
	///@{
	virtual bool isKeyHold(EKeyCode keyCode) const = 0;
	virtual bool isMouseButtonHold(EMouseCode mouseCode) const = 0;
	///@}

	/*!
	Returns cursor absolute position in 2-D Cartesian coordinate system
	(right: +x, top: +y, origin is on the lower-left corner of the
	platform's display).
	*/
	virtual math::Vector2D getCursorPositionPx() const = 0;

	/*!
	Returns cursor movement between last two updates in 2-D Cartesian
	coordinate system (right: +x, top: +y).
	*/
	virtual math::Vector2D getCursorMovementDeltaPx() const = 0;
};

}// end namespace ph::editor
