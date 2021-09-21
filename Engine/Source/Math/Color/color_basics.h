#pragma once

#include "Common/primitive_type.h"
#include "Common/config.h"
#include "Common/assertion.h"
#include "Math/Color/EColorSpace.h"
#include "Math/Color/EReferenceWhite.h"
#include "Math/Color/EColorUsage.h"
#include "Math/Color/EChromaticAdaptation.h"
#include "Utility/utility.h"

#include <array>
#include <cstddef>
#include <concepts>

namespace ph::math
{

template<typename Property>
concept CSpectralSampleProps = requires (std::size_t value)
{
	TNonTypeTemplateArgDummy<std::size_t, Property::NUM_SAMPLES>();
	TNonTypeTemplateArgDummy<std::size_t, Property::MIN_WAVELENGTH_NM>();
	TNonTypeTemplateArgDummy<std::size_t, Property::MAX_WAVELENGTH_NM>();
};

class DefaultSpectralSampleProps final
{
public:
	inline static constexpr std::size_t NUM_SAMPLES       = PH_SPECTRUM_SAMPLED_NUM_SAMPLES;
	inline static constexpr std::size_t MIN_WAVELENGTH_NM = PH_SPECTRUM_SAMPLED_MIN_WAVELENGTH_NM;
	inline static constexpr std::size_t MAX_WAVELENGTH_NM = PH_SPECTRUM_SAMPLED_MAX_WAVELENGTH_NM;
};

using ColorValue = real;

template<typename T, std::size_t N>
using TRawColorValues = std::array<T, N>;

template<typename T>
using TTristimulusValues = TRawColorValues<T, 3>;

/*!
Properties of spectral sample values (such as wavelength range) are specified in @p DefaultSpectralSampleProps.
*/
template<typename T, CSpectralSampleProps Props = DefaultSpectralSampleProps>
using TSpectralSampleValues = TRawColorValues<T, Props::NUM_SAMPLES>;

template<typename T>
using TChromaticityValues = TRawColorValues<T, 2>;

using TristimulusValues    = TTristimulusValues<ColorValue>;
using SpectralSampleValues = TSpectralSampleValues<ColorValue>;
using ChromaticityValues   = TChromaticityValues<ColorValue>;

/*!
Values are for 2-degree standard observer.

References: 
[1] https://en.wikipedia.org/wiki/Standard_illuminant

The source code of Colour python library:
[2] https://github.com/colour-science/colour/blob/v0.3.16/colour/colorimetry/datasets/illuminants/chromaticity_coordinates.py

@note The values listed on Bruce's website (http://www.brucelindbloom.com/index.html?Eqn_RGB_XYZ_Matrix.html)
are for 10-degree standard observer. Those values can NOT be used here as we need 2-degree standard observer here.
*/
template<typename T = ColorValue>
inline TChromaticityValues<T> chromaticity_of(const EReferenceWhite refWhite)
{
	switch(refWhite)
	{
	case EReferenceWhite::A:        return {0.44758, 0.40745};
	case EReferenceWhite::B:        return {0.34842, 0.35161};
	case EReferenceWhite::C:        return {0.31006, 0.31616};
	case EReferenceWhite::D50:      return {0.34570, 0.35850};
	case EReferenceWhite::D55:      return {0.33243, 0.34744};
	case EReferenceWhite::D60:      return {0.321616709705268, 0.337619916550817};
	case EReferenceWhite::D65:      return {0.31270, 0.32900};
	case EReferenceWhite::D75:      return {0.29903, 0.31488};
	case EReferenceWhite::E:        return {1.0 / 3.0, 1.0 / 3.0};
	case EReferenceWhite::F1:       return {0.31310, 0.33710};
	case EReferenceWhite::F2:       return {0.37210, 0.37510};
	case EReferenceWhite::F3:       return {0.40910, 0.39410};
	case EReferenceWhite::F4:       return {0.44020, 0.40310};
	case EReferenceWhite::F5:       return {0.31380, 0.34520};
	case EReferenceWhite::F6:       return {0.37790, 0.38820};
	case EReferenceWhite::F7:       return {0.31290, 0.32920};
	case EReferenceWhite::F8:       return {0.34580, 0.35860};
	case EReferenceWhite::F9:       return {0.37410, 0.37270};
	case EReferenceWhite::F10:      return {0.34580, 0.35880};
	case EReferenceWhite::F11:      return {0.38050, 0.37690};
	case EReferenceWhite::F12:      return {0.43700, 0.40420};
	case EReferenceWhite::LED_B1:   return {0.45600, 0.40780};
	case EReferenceWhite::LED_B2:   return {0.43570, 0.40120};
	case EReferenceWhite::LED_B3:   return {0.37560, 0.37230};
	case EReferenceWhite::LED_B4:   return {0.34220, 0.35020};
	case EReferenceWhite::LED_B5:   return {0.31180, 0.32360};
	case EReferenceWhite::LED_BH1:  return {0.44740, 0.40660};
	case EReferenceWhite::LED_RGB1: return {0.45570, 0.42110};
	case EReferenceWhite::LED_V1:   return {0.45480, 0.40440};
	case EReferenceWhite::LED_V2:   return {0.37810, 0.37750};

	// References: 
	// [1] TB-2014-004: Informative Notes on SMPTE ST 2065-1 ¡V Academy Color Encoding Specification (ACES)
	// https://www.oscars.org/science-technology/aces/aces-documentation
	// [2] TB-2018-001: Derivation of the ACES White Point Chromaticity Coordinates
	// https://www.oscars.org/science-technology/aces/aces-documentation
	case EReferenceWhite::ACES: return {0.32168, 0.33767};

	default: 
		PH_ASSERT_UNREACHABLE_SECTION();
		return {1.0 / 3.0, 1.0 / 3.0};// Same as standard illuminant E
	}
}

/*
For reference whites, set Y value to 1 (the brightest white that a color display supports).
*/
template<typename T = ColorValue>
inline TTristimulusValues<T> CIEXYZ_of_custom_reference_white(const TChromaticityValues<T>& refWhite)
{
	// Reference whites should not have any component being <= 0
	PH_ASSERT_GT(refWhite[0], static_cast<T>(0));
	PH_ASSERT_GT(refWhite[1], static_cast<T>(0));

	const T x = refWhite[0];
	const T y = refWhite[1];

	// Y is set to 1
	const T Y_over_y = static_cast<T>(1) / y;
	return
	{
		Y_over_y * x,
		1.0,
		Y_over_y * (static_cast<T>(1) - x - y)
	};
}

template<typename T = ColorValue>
inline TTristimulusValues<T> CIEXYZ_of(const EReferenceWhite refWhite)
{
	const auto chromaticity = chromaticity_of<T>(refWhite);
	return CIEXYZ_of_custom_reference_white<T>(chromaticity);
}

}// end namespace ph::math
