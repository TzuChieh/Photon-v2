#pragma once

#include "Core/Texture/Function/TTextureFunction.h"
#include "Utility/has_member.h"
#include "Common/assertion.h"

#include <utility>

namespace ph
{

namespace detail::texture
{

template<typename InputType, typename OutputType>
class TConverter final
{
	static_assert(is_buildable<OutputType, InputType>(),
		"<OutputType> must be buildable from <InputType>");

public:
	OutputType operator () (const InputType& inputValue) const
	{
		return OutputType(inputValue);
	}
};

}// end namespace detail::texture

template<
	typename InputType, 
	typename OutputType, 
	typename Function = detail::texture::TConverter<InputType, OutputType>>
class TConversionTexture : public TTextureFunction<
	InputType,
	OutputType,
	Function>
{
public:
	using TConversionTexture::TTextureFunction::TTextureFunction;
};

}// end namespace ph
