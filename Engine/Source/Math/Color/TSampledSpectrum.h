#pragma once

#include "Math/Color/TSpectrumBase.h"
#include "Math/Color/EColorSpace.h"

#include <cstddef>

namespace ph::math
{

template<EColorSpace COLOR_SPACE, typename T, std::size_t N>
class TSampledSpectrum final :
	public TSpectrumBase<TSampledSpectrum<COLOR_SPACE, T, N>, COLOR_SPACE, T, N>
{
private:
	using Base = TSpectrumBase<TTristimulusSpectrum<COLOR_SPACE, T, N>, COLOR_SPACE, T, N>;

protected:
	using Base::m;

public:
	inline TSampledSpectrum() = default;
	inline TSampledSpectrum(const TSampledSpectrum& other) = default;
	inline TSampledSpectrum(TSampledSpectrum&& other) = default;
	inline TSampledSpectrum& operator = (const TSampledSpectrum& rhs) = default;
	inline TSampledSpectrum& operator = (TSampledSpectrum&& rhs) = default;

	using Base::Base;

	template<typename U>
	TSampledSpectrum(const TSampledSpectrum<COLOR_SPACE, U, N>& other);

	static consteval std::size_t numSamples() noexcept;
	static consteval std::size_t minWavelengthNM() noexcept;
	static consteval std::size_t maxWavelengthNM() noexcept;
	static consteval T sampleInverval() noexcept;
	static constexpr std::pair<T, T> wavelengthRangeOf(std::size_t sampleIndex) noexcept;

	template<typename U>
	static TSampledSpectrum makePiecewiseAveraged(
		const T*    wavelengthsNM,
		const T*    values,
		std::size_t numPoints);
};

}// end namespace ph::math

#include "Math/Color/TSampledSpectrum.ipp"
