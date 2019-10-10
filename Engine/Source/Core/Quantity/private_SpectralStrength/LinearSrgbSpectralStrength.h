#pragma once

#include "Core/Quantity/private_SpectralStrength/TAbstractSpectralStrength.h"
#include "Math/TVector3.h"

namespace ph
{

class LinearSrgbSpectralStrength : 
	public TAbstractSpectralStrength<LinearSrgbSpectralStrength, 3>
{
public:
	using Parent = TAbstractSpectralStrength<LinearSrgbSpectralStrength, 3>;

	inline LinearSrgbSpectralStrength() = default;
	inline LinearSrgbSpectralStrength(const LinearSrgbSpectralStrength& other) = default;
	using Parent::Parent;

	math::Vector3R impl_genLinearSrgb(EQuantity valueType) const;
	void impl_setLinearSrgb(const math::Vector3R& rgb, EQuantity valueType);
	void impl_setSampled(const SampledSpectralStrength& sampled, EQuantity valueType);
};

}// end namespace ph
