#pragma once

#include "Core/Quantity/SpectralStrength/TAbstractSpectralStrength.h"
#include "Core/Quantity/spectral_strength_fwd.h"

#include <cstddef>
#include <utility>

namespace ph
{

template<std::size_t N, std::size_t MIN_LAMBDA_NM, std::size_t MAX_LAMBDA_NM>
class TSampledSpectralStrength final : 
	public TAbstractSpectralStrength<TSampledSpectralStrength<N, MIN_LAMBDA_NM, MAX_LAMBDA_NM>, N>
{
public:
	static constexpr std::size_t NUM_INTERVALS      = N;
	static constexpr real        LAMBDA_RANGE_NM    = static_cast<real>(MAX_LAMBDA_NM - MIN_LAMBDA_NM);
	static constexpr real        LAMBDA_INTERVAL_NM = LAMBDA_RANGE_NM / static_cast<real>(N);

	static inline std::pair<real, real> lambdaRangeNmOf(std::size_t index);

public:
	typedef TAbstractSpectralStrength<TSampledSpectralStrength, N> Parent;

	using Parent::TAbstractSpectralStrength;
	TSampledSpectralStrength();
	TSampledSpectralStrength(const TSampledSpectralStrength& other);
	TSampledSpectralStrength(const TArithmeticArray<real, N>& other);
	virtual inline ~TSampledSpectralStrength() override = default;

	using Parent::operator = ;

	Vector3R impl_genLinearSrgb() const;
	void impl_setLinearSrgb(const Vector3R& rgb);
	void impl_setSampled(const TArithmeticArray<real, N>& sampled);
};

}// end namespace ph

#include "Core/Quantity/SpectralStrength/TSampledSpectralStrength.ipp"