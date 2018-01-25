#pragma once

#include "Core/Texture/Function/TTextureFunction.h"
#include "Common/primitive_type.h"

namespace ph
{

template<typename InputType, typename OutputType>
inline TTextureFunction<InputType, OutputType>::TTextureFunction() : 
	TTextureFunction(nullptr)
{

}

template<typename InputType, typename OutputType>
inline TTextureFunction<InputType, OutputType>::TTextureFunction(
	const std::shared_ptr<TTexture<InputType>>& parentTexture) :
	m_parentTexture(parentTexture)
{

}

template<typename InputType, typename OutputType>
inline TTextureFunction<InputType, OutputType>::~TTextureFunction() = default;

template<typename InputType, typename OutputType>
inline void TTextureFunction<InputType, OutputType>::setParentTexture(
	const std::shared_ptr<TTexture<InputType>>& parentTexture)
{
	PH_ASSERT(parentTexture != nullptr);

	m_parentTexture = parentTexture;
}

}// namespace ph