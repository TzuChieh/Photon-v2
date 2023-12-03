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

	std::shared_ptr<TTexture<Image::ArrayType>> genNumericTexture(
		const CookingContext& ctx) override;

	std::shared_ptr<TTexture<math::Spectrum>> genColorTexture(
		const CookingContext& ctx) override;

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
