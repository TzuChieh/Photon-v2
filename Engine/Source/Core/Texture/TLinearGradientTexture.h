#pragma once

#include "Core/Texture/TTexture.h"
#include "Common/primitive_type.h"
#include "Math/TVector2.h"
#include "Math/TVector3.h"

#include <memory>

namespace ph
{

template<typename OutputType>
class TLinearGradientTexture : public TTexture<OutputType>
{
public:
	TLinearGradientTexture(
		real                                         beginU,
		const std::shared_ptr<TTexture<OutputType>>& beginTexture,
		real                                         endU,
		const std::shared_ptr<TTexture<OutputType>>& endTexture);

	TLinearGradientTexture(
		const math::Vector2R&                        beginUV,
		const std::shared_ptr<TTexture<OutputType>>& beginTexture,
		const math::Vector2R&                        endUV,
		const std::shared_ptr<TTexture<OutputType>>& endTexture);

	TLinearGradientTexture(
		const math::Vector3R&                        beginUVW, 
		const std::shared_ptr<TTexture<OutputType>>& beginTexture,
		const math::Vector3R&                        endUVW,
		const std::shared_ptr<TTexture<OutputType>>& endTexture);

	void sample(const SampleLocation& sampleLocation, Output* out_value) const override;

private:
	math::Vector3R                        m_beginUVW;
	std::shared_ptr<TTexture<OutputType>> m_beginTexture;
	math::Vector3R                        m_endUVW;
	std::shared_ptr<TTexture<OutputType>> m_endTexture;
};

}// end namespace ph

#include "Core/Texture/TLinearGradientTexture.ipp"
