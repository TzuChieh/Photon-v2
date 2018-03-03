#pragma once

#include "Core/Quantity/SpectralStrength/TAbstractSpectralStrength.h"

namespace ph
{

class LinearSrgbSpectralStrength final : 
	public TAbstractSpectralStrength<LinearSrgbSpectralStrength, 3>
{
public:
	typedef TAbstractSpectralStrength<LinearSrgbSpectralStrength, 3> Parent;

	LinearSrgbSpectralStrength();
	LinearSrgbSpectralStrength(const LinearSrgbSpectralStrength& other);
	using Parent::TAbstractSpectralStrength;
	virtual inline ~LinearSrgbSpectralStrength() override = default;

	using Parent::operator = ;

	Vector3R impl_genLinearSrgb() const;
	void impl_setLinearSrgb(const Vector3R& linearSrgb);
	void impl_setSampled(const SampledSpectralStrength& sampled);
};

}// end namespace ph