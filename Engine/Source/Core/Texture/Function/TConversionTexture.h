#pragma once

#include "Core/Texture/Function/TTextureFunction.h"
#include "Utility/traits.h"
#include "Common/assertion.h"
#include "Core/Texture/TTexPixel.h"
#include "Math/Color/Spectrum.h"

#include <utility>
#include <cstddef>

namespace ph
{

namespace texture_converter
{

template<typename InputType, typename OutputType>
class TDefault final
{
	static_assert(IsBuildable<OutputType, InputType>(),
		"<OutputType> must be buildable from <InputType>");

public:
	OutputType operator () (const InputType& inputValue) const
	{
		return OutputType(inputValue);
	}
};

template<typename T, std::size_t N>
class TTexPixelToSpectrum final
{
public:
	math::Spectrum operator () (const TTexPixel<T, N>& inputValue) const
	{
		if constexpr(N == 1)
		{
			return math::Spectrum(inputValue[0]);
		}
		else
		{
			static_assert(N == math::Spectrum::NUM_VALUES,
				"Cannot convert mismatched number of components from TexPixel to Spectrum");

			return math::Spectrum(inputValue.toArray());
		}
	}
};

}// end namespace texture_converter

template
<
	typename InputType, 
	typename OutputType, 
	typename Function = texture_converter::TDefault<InputType, OutputType>
>
class TConversionTexture : public TTextureFunction<
	InputType,
	OutputType,
	Function>
{
public:
	// Inherit base class ctor
	using TTextureFunction<
		InputType, 
		OutputType, 
		Function>::TTextureFunction;
};

}// end namespace ph
