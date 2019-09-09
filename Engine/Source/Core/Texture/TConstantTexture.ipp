#pragma once

#include "Core/Texture/TConstantTexture.h"
#include "Common/assertion.h"

namespace ph
{

template<typename OutputType>
inline TConstantTexture<OutputType>::TConstantTexture(const OutputType& value) :
	m_value(value)
{}

template<typename OutputType>
inline void TConstantTexture<OutputType>::sample(
	const SampleLocation& sampleLocation, OutputType* const out_value) const
{
	PH_ASSERT(out_value);

	*out_value = m_value;
}

}// end namespace ph
