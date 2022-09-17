#pragma once

#include <Common/primitive_type.h>
#include <Math/TVector2.h>

namespace ph::editor
{

enum class EKeyCode : uint32f
{
	// Unknow/unsupported key
	Unknown = 0,

	// F<X> keys
	F1  = 1,
	F2  = 2,
	F3  = 3,
	F4  = 4,
	F5  = 5,
	F6  = 6,
	F7  = 7,
	F8  = 8,
	F9  = 9,
	F10 = 10,
	F11 = 11,
	F12 = 12,

	// Characters
	A = 20,
	B = 21,
	C = 22,
	D = 23,
	E = 24,
	F = 25,
	G = 26,
	H = 27,
	I = 28,
	J = 29,
	K = 30,
	L = 31,
	M = 32,
	N = 33,
	O = 34,
	P = 35,
	Q = 36,
	R = 37,
	S = 38,
	T = 39,
	U = 40,
	V = 41,
	W = 42,
	X = 43,
	Y = 44,
	Z = 45,

	Space        = 46,
	Apostrophe   = 47,/* ' */
	Comma        = 48,/* , */
	Minus        = 49,/* - */
	Period       = 50,/* . */
	Slash        = 51,/* / */
	Semicolon    = 52,/* ; */
	Equal        = 53,/* = */
	LeftBracket  = 54,/* [ */
	Backslash    = 55,/* \ */
	RightBracket = 56,/* ] */
	GraveAccent  = 57,/* ` */

	// Common functional keys
	Escape       = 60,
	Enter        = 61,
	Tab          = 62,
	Backspace    = 63,
	Insert       = 64,
	Delete       = 65,
	Right        = 66,
	Left         = 67,
	Down         = 68,
	Up           = 69,
	PageUp       = 70,
	PageDown     = 71,
	Home         = 72,
	End          = 73,
	CapsLock     = 74,
	ScrollLock   = 75,
	NumLock      = 76,
	PrintScreen  = 77,
	Pause        = 78,
	LeftShift    = 79,
	LeftControl  = 80,
	LeftAlt      = 81,
	LeftSuper    = 82,
	RightShift   = 83,
	RightControl = 84,
	RightAlt     = 85,
	RightSuper   = 86,
	Menu         = 87,

	// Keypad keys
	K0        = 90,
	K1        = 91,
	K2        = 92,
	K3        = 93,
	K4        = 94,
	K5        = 95,
	K6        = 96,
	K7        = 97,
	K8        = 98,
	K9        = 99,
	KDecimal  = 100,
	KDivide   = 101,
	KMultiply = 102,
	KSubtract = 103,
	KAdd      = 104,
	KEnter    = 105,
	KEqual    = 106,

	// Number keys (normally below F<X> keys)
	D0 = 110,/* 0 */
	D1 = 111,/* 1 */
	D2 = 112,/* 2 */
	D3 = 113,/* 3 */
	D4 = 114,/* 4 */
	D5 = 115,/* 5 */
	D6 = 116,/* 6 */
	D7 = 117,/* 7 */
	D8 = 118,/* 8 */
	D9 = 119,/* 9 */

	// Number of values
	Num,
};// end EKeyCode

enum class EMouseCode : uint32f
{
	// Unknow/unsupported mouse button
	Unknown = 0,

	// Common mouse buttons
	Left   = 1,
	Right  = 2,
	Middle = 3,

	// Number of values
	Num,
};// end EMouseCode

class Input
{
public:
	inline virtual ~Input() = default;

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
	Returns true if the key/button was detected as released after last update
	and pressed after current update; otherwise, false is returned.
	*/
	///@{
	virtual bool isKeyDown(EKeyCode keyCode) const = 0;
	virtual bool isMouseButtonDown(EMouseCode mouseCode) const = 0;
	///@}

	/*!
	Returns true if the key/button was detected as pressed after last update
	and released after current update; otherwise, false is returned.
	*/
	///@{
	virtual bool isKeyUp(EKeyCode keyCode) const = 0;
	virtual bool isMouseButtonUp(EMouseCode mouseCode) const = 0;
	///@}

	/*!
	Returns true if the key/button is continuously being pressed. Notice that
	this method will return false while is<X>Down() or is<X>Up() is true.
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
