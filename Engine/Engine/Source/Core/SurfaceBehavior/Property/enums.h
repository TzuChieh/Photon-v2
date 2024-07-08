#pragma once

namespace ph
{

/*! @brief Different types of masking and shadowing terms for microfacet distributions.
Eric Heitz has published a detailed explanation @cite Heitz:2014:Microfacet, which is excellent and
definitely worth a read in my opinion.
*/
enum class EMaskingShadowing
{
	/*! Modeling the correlation between masking and shadowing due to the height of the microsurface.
	This is a more accurate version than the one used by Walter et al. @cite Walter:2007:Microfacet
	(which is `Separable`).
	*/
	HightCorrelated = 0,

	/*! Statistically independent masking and shadowing. This form is simpelr but always overestimates
	the geometry term as some correlation always exists.
	*/
	Separable,

	/*! Modeling the correlation between masking and shadowing due to the differences between incident
	and outgoing directions.
	*/
	DirectionCorrelated,

	/*! Modeling the correlation between masking and shadowing due to both height and direction
	differences. This is the most accurate version of all.
	*/
	HeightDirectionCorrelated
};

}// end namespace ph
