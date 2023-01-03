#pragma once

#include "Common/primitive_type.h"
#include "Common/config.h"
#include "Common/assertion.h"
#include "Math/Color/color_enums.h"
#include "Utility/utility.h"

#include <array>
#include <cstddef>
#include <concepts>

namespace ph::math
{

template<typename Property>
concept CSpectralSampleProps = requires
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
Properties of spectral sample values (such as wavelength range) are specified in @p SampleProps.
*/
template<typename T, CSpectralSampleProps SampleProps = DefaultSpectralSampleProps>
using TSpectralSampleValues = TRawColorValues<T, SampleProps::NUM_SAMPLES>;

template<typename T>
using TChromaticityValues = TRawColorValues<T, 2>;

using TristimulusValues    = TTristimulusValues<ColorValue>;
using SpectralSampleValues = TSpectralSampleValues<ColorValue>;
using ChromaticityValues   = TChromaticityValues<ColorValue>;

namespace detail
{

/*!
Values are for 2-degree standard observer.

References: 
[1] https://en.wikipedia.org/wiki/Standard_illuminant

The source code of Colour python library:
[2] https://github.com/colour-science/colour/blob/v0.3.16/colour/colorimetry/datasets/illuminants/chromaticity_coordinates.py

@note The values listed on Bruce's website (http://www.brucelindbloom.com/index.html?Eqn_RGB_XYZ_Matrix.html)
are for 10-degree standard observer. Those values can NOT be used here as we need 2-degree standard observer here.
*/
template<typename T>
inline auto make_chromaticity_table()
-> std::array<TChromaticityValues<T>, static_cast<std::size_t>(EReferenceWhite::NUM)>
{
	std::array<TChromaticityValues<double>, static_cast<std::size_t>(EReferenceWhite::NUM)> table;
	
	// By default fill all values as standard illuminant E
	table.fill({1.0 / 3.0, 1.0 / 3.0});

	table[static_cast<std::size_t>(EReferenceWhite::A)]        = {0.44758, 0.40745};
	table[static_cast<std::size_t>(EReferenceWhite::B)]        = {0.34842, 0.35161};
	table[static_cast<std::size_t>(EReferenceWhite::C)]        = {0.31006, 0.31616};
	table[static_cast<std::size_t>(EReferenceWhite::D50)]      = {0.34570, 0.35850};
	table[static_cast<std::size_t>(EReferenceWhite::D55)]      = {0.33243, 0.34744};
	table[static_cast<std::size_t>(EReferenceWhite::D60)]      = {0.321616709705268, 0.337619916550817};
	table[static_cast<std::size_t>(EReferenceWhite::D65)]      = {0.31270, 0.32900};
	table[static_cast<std::size_t>(EReferenceWhite::D75)]      = {0.29903, 0.31488};
	table[static_cast<std::size_t>(EReferenceWhite::E)]        = {1.0 / 3.0, 1.0 / 3.0};
	table[static_cast<std::size_t>(EReferenceWhite::F1)]       = {0.31310, 0.33710};
	table[static_cast<std::size_t>(EReferenceWhite::F2)]       = {0.37210, 0.37510};
	table[static_cast<std::size_t>(EReferenceWhite::F3)]       = {0.40910, 0.39410};
	table[static_cast<std::size_t>(EReferenceWhite::F4)]       = {0.44020, 0.40310};
	table[static_cast<std::size_t>(EReferenceWhite::F5)]       = {0.31380, 0.34520};
	table[static_cast<std::size_t>(EReferenceWhite::F6)]       = {0.37790, 0.38820};
	table[static_cast<std::size_t>(EReferenceWhite::F7)]       = {0.31290, 0.32920};
	table[static_cast<std::size_t>(EReferenceWhite::F8)]       = {0.34580, 0.35860};
	table[static_cast<std::size_t>(EReferenceWhite::F9)]       = {0.37410, 0.37270};
	table[static_cast<std::size_t>(EReferenceWhite::F10)]      = {0.34580, 0.35880};
	table[static_cast<std::size_t>(EReferenceWhite::F11)]      = {0.38050, 0.37690};
	table[static_cast<std::size_t>(EReferenceWhite::F12)]      = {0.43700, 0.40420};
	table[static_cast<std::size_t>(EReferenceWhite::LED_B1)]   = {0.45600, 0.40780};
	table[static_cast<std::size_t>(EReferenceWhite::LED_B2)]   = {0.43570, 0.40120};
	table[static_cast<std::size_t>(EReferenceWhite::LED_B3)]   = {0.37560, 0.37230};
	table[static_cast<std::size_t>(EReferenceWhite::LED_B4)]   = {0.34220, 0.35020};
	table[static_cast<std::size_t>(EReferenceWhite::LED_B5)]   = {0.31180, 0.32360};
	table[static_cast<std::size_t>(EReferenceWhite::LED_BH1)]  = {0.44740, 0.40660};
	table[static_cast<std::size_t>(EReferenceWhite::LED_RGB1)] = {0.45570, 0.42110};
	table[static_cast<std::size_t>(EReferenceWhite::LED_V1)]   = {0.45480, 0.40440};
	table[static_cast<std::size_t>(EReferenceWhite::LED_V2)]   = {0.37810, 0.37750};

	// References: 
	// [1] TB-2014-004: Informative Notes on SMPTE ST 2065-1 – Academy Color Encoding Specification (ACES)
	// https://www.oscars.org/science-technology/aces/aces-documentation
	// [2] TB-2018-001: Derivation of the ACES White Point Chromaticity Coordinates
	// https://www.oscars.org/science-technology/aces/aces-documentation
	table[static_cast<std::size_t>(EReferenceWhite::ACES)] = {0.32168, 0.33767};

	std::array<TChromaticityValues<T>, static_cast<std::size_t>(EReferenceWhite::NUM)> castedTable;
	for(std::size_t i = 0; i < table.size(); ++i)
	{
		castedTable[i][0] = static_cast<T>(table[i][0]);
		castedTable[i][1] = static_cast<T>(table[i][1]);
	}
	return castedTable;
}

}// end namespace detail

template<typename T = ColorValue>
inline TChromaticityValues<T> chromaticity_of(const EReferenceWhite refWhite)
{
	static const auto TABLE = detail::make_chromaticity_table<T>();

	const std::size_t index = static_cast<std::size_t>(refWhite);
	PH_ASSERT_LT(index, TABLE.size());
	return TABLE[index];
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
