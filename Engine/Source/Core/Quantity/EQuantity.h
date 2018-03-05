#pragma once

namespace ph
{

enum class EQuantity
{
	// Raw data such as position, normal, density information.
	//
	RAW,

	// EMR stands for Electromagnetic Radiation; the quantity typical light 
	// sources emitted.
	// 
	EMR,

	// ECF stands for Energy Conservative Fraction; surface albedo, 
	// reflectance, transmittance... are all ECFs. It is required that an ECF
	// have value within [0, 1].
	ECF
};

}// end namespace ph