#pragma once

#include "Core/Quantity/private_SpectralStrength/TAbstractSpectralStrength.h"
#include "Core/Quantity/spectral_strength_fwd.h"
#include "Math/TVector3.h"

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
	using Parent = TAbstractSpectralStrength<TSampledSpectralStrength, N>;

	inline TSampledSpectralStrength() = default;
	inline TSampledSpectralStrength(const TSampledSpectralStrength& other) = default;
	using Parent::Parent;

	//void asAveraged(const real* lambdasNm, const real* strengths, std::size_t numElements);

	inline Vector3R impl_genLinearSrgb(EQuantity valueType) const;
	inline void impl_setLinearSrgb(const Vector3R& rgb, EQuantity valueType);
	inline void impl_setSampled(const SampledSpectralStrength& sampled, EQuantity valueType);
};

}// end namespace ph