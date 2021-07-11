#pragma once

#include "Actor/Image/Image.h"

#include <memory>
#include <utility>

namespace ph
{

class BinaryMixedImage : public Image
{
public:
	BinaryMixedImage();

	std::shared_ptr<TTexture<real>> genTextureReal(
		ActorCookingContext& ctx) const override = 0;

	std::shared_ptr<TTexture<math::Vector3R>> genTextureVector3R(
		ActorCookingContext& ctx) const override = 0;

	std::shared_ptr<TTexture<Spectrum>> genTextureSpectral(
		ActorCookingContext& ctx) const override = 0;

	void setImageA(const std::weak_ptr<Image>& imageA);
	void setImageB(const std::weak_ptr<Image>& imageB);

protected:
	auto checkoutImages() const->std::pair<std::shared_ptr<Image>, std::shared_ptr<Image>>;

private:
	std::weak_ptr<Image> m_imageA;
	std::weak_ptr<Image> m_imageB;
};

// In-header Implementations:

inline void BinaryMixedImage::setImageA(const std::weak_ptr<Image>& imageA)
{
	m_imageA = imageA;
}

inline void BinaryMixedImage::setImageB(const std::weak_ptr<Image>& imageB)
{
	m_imageB = imageB;
}

}// end namespace ph
