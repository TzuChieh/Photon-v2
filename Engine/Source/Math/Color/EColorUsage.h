#pragma once

namespace ph::math
{

enum class EColorUsage
{
	UNSPECIFIED = 0,

	/*!
	Raw data such as position, normal, density information.
	*/
	RAW,

	/*!
	EMR stands for ElectroMagnetic Radiation; the quantity typical light 
	sources emitted.
	*/
	EMR,

	/*!
	ECF stands for Energy Conservative Fraction; surface albedo, 
	reflectance, transmittance... are all ECFs. It is required that an ECF
	have value within [0, 1].
	*/
	ECF
};

}// end namespace ph::math
