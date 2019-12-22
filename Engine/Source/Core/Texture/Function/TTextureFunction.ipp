#pragma once

#include "Core/Texture/Function/TTextureFunction.h"
#include "Common/primitive_type.h"
#include "Core/Texture/SampleLocation.h"

#include <memory>

namespace ph
{


template<typename InputType, typename OutputType, typename Function>
template<typename>
inline TTextureFunction<InputType, OutputType, Function>::
TTextureFunction(InputTextureResource inputTexture) : 
	TTextureFunction(std::move(inputTexture), Function())
{}

template<typename InputType, typename OutputType, typename Function>
inline TTextureFunction<InputType, OutputType, Function>::
TTextureFunction(InputTextureResource inputTexture, Function function) : 
	m_inputTexture(std::move(inputTexture)),
	m_function    (std::move(function))
{}

template<typename InputType, typename OutputType, typename Function>
inline void TTextureFunction<InputType, OutputType, Function>::
sample(const SampleLocation& sampleLocation, OutputType* const out_value) const
{
	PH_ASSERT(m_inputTexture);
	PH_ASSERT(out_value);

	InputType inputValue;
	m_inputTexture->sample(sampleLocation, &inputValue);

	*out_value = m_function(inputValue);
}

}// namespace ph
