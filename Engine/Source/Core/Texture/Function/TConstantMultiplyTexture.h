#pragma once

#include "Core/Texture/Function/TTextureFunction.h"
#include "Utility/has_member.h"

#include <utility>
#include <memory>

namespace ph
{

namespace detail::texture
{

template<typename InputType, typename ConstantType, typename OutputType>
class TConstantMultiplier final
{
	static_assert(has_multiply_operator<InputType, ConstantType, OutputType>::value,
		"Must have multiply operator for <OutputType> = <InputType> * <ConstantType>");

public:
	explicit TConstantMultiplier(ConstantType constant) :
		m_constant(std::move(constant))
	{}

	OutputType operator () (const InputType& inputValue) const
	{
		return inputValue * m_constant;
	}

private:
	ConstantType m_constant;
};

}// end namespace detail::texture

template<
	typename InputType, 
	typename ConstantType, 
	typename OutputType,
	typename Function = detail::texture::TConstantMultiplier<InputType, ConstantType, OutputType>>
class TConstantMultiplyTexture : public TTextureFunction<
	InputType,
	OutputType,
	Function>
{
public:
	TConstantMultiplyTexture::TConstantMultiplyTexture(
		std::shared_ptr<TTexture<InputType>> inputTexture,
		ConstantType                         constantValue) : 

		TConstantMultiplyTexture::TTextureFunction(
			std::move(inputTexture), 
			Function(std::move(constantValue)))
	{}
};

}// end namespace ph
