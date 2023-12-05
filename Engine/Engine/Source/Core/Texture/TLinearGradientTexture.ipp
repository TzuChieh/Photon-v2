#pragma once

#include "Core/Texture/TLinearGradientTexture.h"
#include "Core/Texture/SampleLocation.h"

#include <Common/assertion.h>

namespace ph
{

template<typename OutputType>
inline TLinearGradientTexture<OutputType>::TLinearGradientTexture(
	const real                                   beginU,
	const std::shared_ptr<TTexture<OutputType>>& beginTexture,
	const real                                   endU,
	const std::shared_ptr<TTexture<OutputType>>& endTexture) :

	TLinearGradientTexture(
		{beginU, 0, 0},
		beginTexture, 
		{endU, 0, 0},
		endTexture)
{}

template<typename OutputType>
inline TLinearGradientTexture<OutputType>::TLinearGradientTexture(
	const math::Vector2R&                        beginUV,
	const std::shared_ptr<TTexture<OutputType>>& beginTexture,
	const math::Vector2R&                        endUV,
	const std::shared_ptr<TTexture<OutputType>>& endTexture) :

	TLinearGradientTexture(
		{beginUV.x, beginUV.y, 0},
		beginTexture, 
		{endUV.x, endUV.y, 0},
		endTexture)
{}

template<typename OutputType>
inline TLinearGradientTexture<OutputType>::TLinearGradientTexture(
	const math::Vector3R&                        beginUVW,
	const std::shared_ptr<TTexture<OutputType>>& beginTexture,
	const math::Vector3R&                        endUVW,
	const std::shared_ptr<TTexture<OutputType>>& endTexture) :

	TTexture<OutputType>(),

	m_beginUVW    (beginUVW),
	m_beginTexture(beginTexture),
	m_endUVW      (endUVW),
	m_endTexture  (endTexture)
{
	PH_ASSERT(!beginUVW.isEqual(endUVW));
	PH_ASSERT(beginTexture);
	PH_ASSERT(endTexture);
}

template<typename OutputType>
inline void TLinearGradientTexture<OutputType>::sample(const SampleLocation& sampleLocation, OutputType* const out_value) const
{
	PH_ASSERT(out_value);
	PH_ASSERT(m_beginTexture);
	PH_ASSERT(m_beginTexture.get() != this);
	PH_ASSERT(m_endTexture);
	PH_ASSERT(m_endTexture.get() != this);

	const auto baseVec       = m_endUVW - m_beginUVW;
	const real baseLength    = baseVec.length();
	const real rcpBaseLength = 1.0_r / baseLength;
	const auto baseAxis      = baseVec * rcpBaseLength;
	const auto sampleVec     = sampleLocation.uvw() - m_beginUVW;
	const real sampleLength  = sampleVec.dot(baseAxis);

	// Lerp between endpoints

	const real beginRatio = (baseLength - sampleLength) * rcpBaseLength;
	const real endRatio   = sampleLength * rcpBaseLength;

	OutputType beginValue, endValue;
	m_beginTexture->sample(sampleLocation, &beginValue);
	m_endTexture->sample(sampleLocation, &endValue);

	*out_value = beginValue * beginRatio + endValue * endRatio;
}

}// end namespace ph
