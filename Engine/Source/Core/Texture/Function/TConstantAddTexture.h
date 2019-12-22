#pragma once

#include "Core/Texture/Function/TTextureFunction.h"
#include "Utility/has_member.h"

#include <utility>

namespace ph
{

namespace detail::texture
{

template<typename InputType, typename ConstantType, typename OutputType>
class TConstantAdder final
{
	static_assert(has_add_operator<InputType, ConstantType, OutputType>::value,
		"Must have add operator for <OutputType> = <InputType> + <ConstantType>");

public:
	explicit TConstantAdder(ConstantType constant) :
		m_constant(std::move(constant))
	{}

	OutputType operator () (const InputType& inputValue) const
	{
		return inputValue + m_constant;
	}

private:
	ConstantType m_constant;
};

}// end namespace detail::texture

template<
	typename InputType, 
	typename ConstantType, 
	typename OutputType,
	typename Function = detail::texture::TConstantAdder<InputType, ConstantType, OutputType>>
class TConstantAddTexture : public TTextureFunction<
	InputType,
	OutputType,
	Function>
{
public:
	TConstantAddTexture::TConstantAddTexture(
		std::shared_ptr<TTexture<InputType>> inputTexture,
		ConstantType                         constantValue) :

		TConstantAddTexture::TTextureFunction(
			std::move(inputTexture), 
			Function(std::move(constantValue)))
	{}
};

}// end namespace ph
