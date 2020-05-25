#pragma once

#include "Actor/Image/Image.h"

#include <memory>
#include <utility>

namespace ph
{

class BinaryMixedImage : public Image, public TCommandInterface<BinaryMixedImage>
{
public:
	BinaryMixedImage();

	std::shared_ptr<TTexture<real>> genTextureReal(
		CookingContext& context) const override = 0;

	std::shared_ptr<TTexture<math::Vector3R>> genTextureVector3R(
		CookingContext& context) const override = 0;

	std::shared_ptr<TTexture<Spectrum>> genTextureSpectral(
		CookingContext& context) const override = 0;

	void setImageA(const std::weak_ptr<Image>& imageA);
	void setImageB(const std::weak_ptr<Image>& imageB);

protected:
	auto checkoutImages() const->std::pair<std::shared_ptr<Image>, std::shared_ptr<Image>>;

private:
	std::weak_ptr<Image> m_imageA;
	std::weak_ptr<Image> m_imageB;

// command interface
public:
	explicit BinaryMixedImage(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
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
