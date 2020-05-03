#pragma once

#include "Core/Quantity/private_Spectrum/TAbstractSpectrum.h"
#include "Core/Quantity/spectrum_fwd.h"
#include "Math/TVector3.h"

#include <cstddef>
#include <utility>

namespace ph
{

template<std::size_t N, std::size_t MIN_LAMBDA_NM, std::size_t MAX_LAMBDA_NM>
class TSampledSpectrum : 
	public TAbstractSpectrum<TSampledSpectrum<N, MIN_LAMBDA_NM, MAX_LAMBDA_NM>, N>
{
public:
	static constexpr std::size_t NUM_INTERVALS      = N;
	static constexpr real        LAMBDA_RANGE_NM    = static_cast<real>(MAX_LAMBDA_NM - MIN_LAMBDA_NM);
	static constexpr real        LAMBDA_INTERVAL_NM = LAMBDA_RANGE_NM / static_cast<real>(N);

	static inline std::pair<real, real> lambdaRangeNmOf(std::size_t index);

public:
	using Parent = TAbstractSpectrum<TSampledSpectrum, N>;

	inline TSampledSpectrum() = default;
	inline TSampledSpectrum(const TSampledSpectrum& other) = default;
	using Parent::Parent;

	//void asAveraged(const real* lambdasNm, const real* strengths, std::size_t numElements);

	inline math::Vector3R impl_genLinearSrgb(EQuantity valueType) const;
	inline void impl_setLinearSrgb(const math::Vector3R& rgb, EQuantity valueType);
	inline void impl_setSampled(const SampledSpectrum& sampled, EQuantity valueType);
};

}// end namespace ph
