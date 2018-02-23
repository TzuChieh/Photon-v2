#pragma once

#include "Core/Quantity/SpectralStrength/TAbstractSpectralStrength.h"

namespace ph
{

class LinearSrgbSpectralStrength final : 
	public TAbstractSpectralStrength<LinearSrgbSpectralStrength, 3>
{
public:
	typedef TAbstractSpectralStrength<LinearSrgbSpectralStrength, 3> Parent;

	using Parent::TAbstractSpectralStrength;
	LinearSrgbSpectralStrength();
	LinearSrgbSpectralStrength(const LinearSrgbSpectralStrength& other);
	LinearSrgbSpectralStrength(const TArithmeticArray<real, 3>& other);
	virtual inline ~LinearSrgbSpectralStrength() override = default;

	using Parent::operator = ;

	Vector3R impl_genLinearSrgb() const;
	void impl_setLinearSrgb(const Vector3R& linearSrgb);
	void impl_setSampled(const TArithmeticArray<real, 3>& sampled);
};

}// end namespace ph