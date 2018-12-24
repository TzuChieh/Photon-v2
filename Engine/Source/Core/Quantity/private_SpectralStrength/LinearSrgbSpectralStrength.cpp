#include "Core/Quantity/private_SpectralStrength/LinearSrgbSpectralStrength.h"
#include "Core/Quantity/SpectralStrength.h"
#include "Core/Quantity/ColorSpace.h"

namespace ph
{

Vector3R LinearSrgbSpectralStrength::impl_genLinearSrgb(const EQuantity valueType) const
{
	return Vector3R(m_values[0], m_values[1], m_values[2]);
}

void LinearSrgbSpectralStrength::impl_setLinearSrgb(const Vector3R& linearSrgb, const EQuantity valueType)
{
	m_values[0] = linearSrgb.x;
	m_values[1] = linearSrgb.y;
	m_values[2] = linearSrgb.z;
}

void LinearSrgbSpectralStrength::impl_setSampled(const SampledSpectralStrength& sampled, const EQuantity valueType)
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