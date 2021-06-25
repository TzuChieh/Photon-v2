#pragma once

#include "Actor/Image/BinaryMixedImage.h"
#include "Math/TVector3.h"

namespace ph
{

class GradientImage : public BinaryMixedImage
{
public:
	GradientImage();

	std::shared_ptr<TTexture<real>> genTextureReal(
		CookingContext& context) const override;

	std::shared_ptr<TTexture<math::Vector3R>> genTextureVector3R(
		CookingContext& context) const override;

	std::shared_ptr<TTexture<Spectrum>> genTextureSpectral(
		CookingContext& context) const override;

	void setUvwEndpoints(const math::Vector3R& beginUvw, const math::Vector3R& endUvw);

private:
	math::Vector3R m_beginUvw;
	math::Vector3R m_endUvw;
};

}// end namespace ph
