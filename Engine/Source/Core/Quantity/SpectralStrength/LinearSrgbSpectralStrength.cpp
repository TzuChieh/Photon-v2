#include "Core/Quantity/SpectralStrength/LinearSrgbSpectralStrength.h"
#include "Core/Quantity/SpectralStrength/TSampledSpectralStrength.h"

namespace ph
{

LinearSrgbSpectralStrength::LinearSrgbSpectralStrength() :
	Parent()
{}

LinearSrgbSpectralStrength::LinearSrgbSpectralStrength(const LinearSrgbSpectralStrength& other) :
	Parent(other)
{}

Vector3R LinearSrgbSpectralStrength::impl_genLinearSrgb() const
{
	return Vector3R(m_values[0], m_values[1], m_values[2]);
}

void LinearSrgbSpectralStrength::impl_setLinearSrgb(const Vector3R& linearSrgb)
{
	m_values[0] = linearSrgb.x;
	m_values[1] = linearSrgb.y;
	m_values[2] = linearSrgb.z;
}

void LinearSrgbSpectralStrength::impl_setSampled(const SampledSpectralStrength& sampled)
{
	impl_setLinearSrgb(ColorSpace::SPD_to_linear_sRGB(sampled));
}

}// end namespace ph