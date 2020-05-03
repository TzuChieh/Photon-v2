#include "Core/Quantity/private_Spectrum/LinearSrgbSpectrum.h"
#include "Core/Quantity/Spectrum.h"
#include "Core/Quantity/ColorSpace.h"

namespace ph
{

math::Vector3R LinearSrgbSpectrum::impl_genLinearSrgb(const EQuantity valueType) const
{
	return math::Vector3R(m_values[0], m_values[1], m_values[2]);
}

void LinearSrgbSpectrum::impl_setLinearSrgb(const math::Vector3R& linearSrgb, const EQuantity valueType)
{
	m_values[0] = linearSrgb.x;
	m_values[1] = linearSrgb.y;
	m_values[2] = linearSrgb.z;
}

void LinearSrgbSpectrum::impl_setSampled(const SampledSpectrum& sampled, const EQuantity valueType)
{
	switch(valueType)
	{
	case EQuantity::EMR:
		impl_setLinearSrgb(ColorSpace::SPD_to_linear_sRGB<ESourceHint::ILLUMINANT>(sampled), valueType);
		break;

	case EQuantity::ECF:
		impl_setLinearSrgb(ColorSpace::SPD_to_linear_sRGB<ESourceHint::REFLECTANCE>(sampled), valueType);
		break;

	default:
		impl_setLinearSrgb(ColorSpace::SPD_to_linear_sRGB<ESourceHint::RAW_DATA>(sampled), valueType);
		break;
	}
}

}// end namespace ph
