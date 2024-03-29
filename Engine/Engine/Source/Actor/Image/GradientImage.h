#pragma once

#include "Actor/Image/BinaryMixedImage.h"
#include "Math/TVector3.h"

namespace ph
{

class GradientImage : public BinaryMixedImage
{
public:
	GradientImage();

	std::shared_ptr<TTexture<Image::ArrayType>> genNumericTexture(
		const CookingContext& ctx) override;

	std::shared_ptr<TTexture<math::Spectrum>> genColorTexture(
		const CookingContext& ctx) override;

	void setUvwEndpoints(const math::Vector3R& beginUvw, const math::Vector3R& endUvw);

private:
	math::Vector3R m_beginUvw;
	math::Vector3R m_endUvw;
};

}// end namespace ph
