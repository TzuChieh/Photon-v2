#include "Core/Quantity/Private/RgbSpectralStrength.h"

namespace ph
{

RgbSpectralStrength::RgbSpectralStrength() : 
	TAbstractSpectralStrength<RgbSpectralStrength, 3>()
{

}

RgbSpectralStrength::RgbSpectralStrength(const RgbSpectralStrength& other) : 
	TAbstractSpectralStrength<RgbSpectralStrength, 3>(other)
{

}

RgbSpectralStrength::RgbSpectralStrength(const TArithmeticArray<real, 3>& other) : 
	TAbstractSpectralStrength<RgbSpectralStrength, 3>(other)
{

}

Vector3R RgbSpectralStrength::impl_genRgb() const
{
	return Vector3R(m[0], m[1], m[2]);
}

void RgbSpectralStrength::impl_setRgb(const Vector3R& rgb)
{
	m[0] = rgb.x;
	m[1] = rgb.y;
	m[2] = rgb.z;
}

void RgbSpectralStrength::impl_setSampled(const std::vector<SpectralSample>& samples)
{
	// TODO
}

}// end namespace ph