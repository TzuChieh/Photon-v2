#pragma once

#include "Actor/Image/Image.h"
#include "SDL/sdl_interface.h"

#include <Common/primitive_type.h>

#include <memory>
#include <functional>
#include <utility>

namespace ph
{

class CheckerboardImage : public Image
{
public:
	std::shared_ptr<TTexture<Image::ArrayType>> genNumericTexture(
		const CookingContext& ctx) override;

	std::shared_ptr<TTexture<math::Spectrum>> genColorTexture(
		const CookingContext& ctx) override;

	void setNumTiles(real numTilesU, real numTilesV);
	void setOddImage(std::shared_ptr<Image> oddImage);
	void setEvenImage(std::shared_ptr<Image> evenImage);

private:
	auto getOddAndEvenImages() const -> std::pair<std::shared_ptr<Image>, std::shared_ptr<Image>>;

	std::shared_ptr<Image> m_oddImage;
	std::shared_ptr<Image> m_evenImage;
	real                   m_numTilesU;
	real                   m_numTilesV;

public:
	PH_DEFINE_SDL_CLASS(TSdlOwnerClass<CheckerboardImage>)
	{
		ClassType clazz("checkerboard");
		clazz.docName("Checkerboard Image");
		clazz.description(
			"A procedural image that divides its parametric domain into rectangular tiles. Each tile "
			"can be categorized into even and odd, where all neighboring tiles of an even tile is odd "
			"and all neighboring tiles of an odd tile is even.");
		clazz.baseOn<Image>();
		
		TSdlReference<Image, OwnerType> oddImage("odd", &OwnerType::m_oddImage);
		oddImage.description(
			"The image to use in the odd tile.");
		oddImage.required();
		clazz.addField(oddImage);

		TSdlReference<Image, OwnerType> evenImage("even", &OwnerType::m_evenImage);
		evenImage.description(
			"The image to use in the even tile.");
		evenImage.required();
		clazz.addField(evenImage);

		TSdlReal<OwnerType> uTiles("u-tiles", &OwnerType::m_numTilesU);
		uTiles.description(
			"Number of tiles in the U axis of the parametric coordinates.");
		uTiles.defaultTo(2.0_r);
		uTiles.optional();
		clazz.addField(uTiles);

		TSdlReal<OwnerType> vTiles("v-tiles", &OwnerType::m_numTilesV);
		vTiles.description(
			"Number of tiles in the V axis of the parametric coordinates.");
		vTiles.defaultTo(2.0_r);
		vTiles.optional();
		clazz.addField(vTiles);

		return clazz;
	}
};

}// end namespace ph
