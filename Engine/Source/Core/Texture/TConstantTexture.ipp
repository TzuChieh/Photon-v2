#pragma once

#include "Core/Texture/TConstantTexture.h"

namespace ph
{

template<typename OutputType>
TConstantTexture<OutputType>::TConstantTexture(const OutputType& value) :
	m_value(value)
{

}

template<typename OutputType>
TConstantTexture<OutputType>::~TConstantTexture() = default;

template<typename OutputType>
void TConstantTexture<OutputType>::sample(const Vector3R& uvw, OutputType* const out_value) const
{
	*out_value = m_value;
}

}// end namespace ph