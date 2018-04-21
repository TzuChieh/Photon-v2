#pragma once

#include "Core/Texture/Function/TTextureFunction.h"
#include "Common/primitive_type.h"

namespace ph
{

template<typename InputType, typename OutputType>
inline TTextureFunction<InputType, OutputType>::TTextureFunction() : 
	m_inputTexture(nullptr)
{}

template<typename InputType, typename OutputType>
inline TTextureFunction<InputType, OutputType>::~TTextureFunction() = default;

template<typename InputType, typename OutputType>
inline auto TTextureFunction<InputType, OutputType>::setInputTexture(
	const std::shared_ptr<TTexture<InputType>>& inputTexture)
	-> TTextureFunction<InputType, OutputType>&
{
	PH_ASSERT(inputTexture != nullptr);
	m_inputTexture = inputTexture;

	return *this;
}

}// namespace ph