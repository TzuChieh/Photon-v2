#pragma once

namespace ph::math
{

enum class EReferenceWhite
{
	UNSPECIFIED = 0,

	// CIE Illuminant A
	A,

	// CIE Illuminant B
	B,

	// CIE Illuminant C
	C,

	// Illuminant D Series: represent natural light
	D50,
	D55,
	D60,
	D65,
	D75,

	// Eual-energy radiator
	E,

	// Illuminant F Series: various types of fluorescent lighting
	F1,
	F2,
	F3,
	F4,
	F5,
	F6,
	F7,
	F8,
	F9,
	F10,
	F11,
	F12,

	// Illuminant Series LED: different LED types (approx. 2700 K to 6600 K)
	LED_B1,
	LED_B2,
	LED_B3,
	LED_B4,
	LED_B5,
	LED_BH1,
	LED_RGB1,
	LED_V1,
	LED_V2,

	// Custom White Points

	/*! 
	The Academy Color Encoding System (ACES) white point, it is close to D60 
	(but not D60! many implementations got this wrong). 
	*/
	ACES
};

}// end namespace ph::math
