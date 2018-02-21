#pragma once

#include "Core/Quantity/SpectralStrength/TAbstractSpectralStrength.h"

#include <cstddef>

namespace ph
{

template<std::size_t N>
class TSampledSpectralStrength final : public TAbstractSpectralStrength<TSampledSpectralStrength<N>, N>
{
public:
	typedef TAbstractSpectralStrength<TSampledSpectralStrength<N>, N> Parent;

	using Parent::TAbstractSpectralStrength;
	TSampledSpectralStrength(const TSampledSpectralStrength& other);
	TSampledSpectralStrength(const TArithmeticArray<real, N>& other);
	virtual inline ~TSampledSpectralStrength() override = default;

	using Parent::operator = ;

	Vector3R impl_genRgb() const;
	void impl_setRgb(const Vector3R& rgb);
};

}// end namespace ph

#include "Core/Quantity/SpectralStrength/TSampledSpectralStrength.ipp"