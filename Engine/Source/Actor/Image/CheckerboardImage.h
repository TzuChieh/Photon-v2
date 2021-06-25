#pragma once

#include "Actor/Image/Image.h"
#include "Common/primitive_type.h"

#include <memory>
#include <functional>
#include <utility>

namespace ph
{

class CheckerboardImage : public Image
{
public:
	CheckerboardImage();

	std::shared_ptr<TTexture<real>> genTextureReal(
		CookingContext& context) const override;

	std::shared_ptr<TTexture<math::Vector3R>> genTextureVector3R(
		CookingContext& context) const override;

	std::shared_ptr<TTexture<Spectrum>> genTextureSpectral(
		CookingContext& context) const override;

	void setNumTiles(real numTilesX, real numTilesY);
	void setOddImage(const std::weak_ptr<Image>& oddImage);
	void setEvenImage(const std::weak_ptr<Image>& evenImage);

private:
	real                 m_numTilesX;
	real                 m_numTilesY;
	std::weak_ptr<Image> m_oddImage;
	std::weak_ptr<Image> m_evenImage;

	auto checkOutImages() const -> std::pair<std::shared_ptr<Image>, std::shared_ptr<Image>>;
};

}// end namespace ph
