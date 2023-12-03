#pragma once

#include "Actor/Image/Image.h"
#include "Actor/Image/sdl_image_enums.h"
#include "Math/TVector2.h"
#include "Common/primitive_type.h"
#include "SDL/sdl_interface.h"

namespace ph
{

class RasterImageBase : public Image
{
public:
	std::shared_ptr<TTexture<Image::ArrayType>> genNumericTexture(
		const CookingContext& ctx) override = 0;

	std::shared_ptr<TTexture<math::Spectrum>> genColorTexture(
		const CookingContext& ctx) override  = 0;

	math::TVector2<uint32> getResolution() const;
	EImageSampleMode getSampleMode() const;
	EImageWrapMode getHorizontalWrapMode() const;
	EImageWrapMode getVerticalWrapMode() const;
	RasterImageBase& setSampleMode(EImageSampleMode mode);
	RasterImageBase& setWrapMode(EImageWrapMode mode);
	RasterImageBase& setWrapMode(EImageWrapMode horizontalWrapMode, EImageWrapMode verticalWrapMode);

protected:
	RasterImageBase& setResolution(math::TVector2<uint32> resolution);

private:
	math::TVector2<uint32> m_resolution;
	EImageSampleMode       m_sampleMode;
	EImageWrapMode         m_wrapMode;
	EImageWrapMode         m_verticalWrapMode;

public:
	PH_DEFINE_SDL_CLASS(TSdlOwnerClass<RasterImageBase>)
	{
		ClassType clazz("raster-base");
		clazz.docName("Base of Raster Image");
		clazz.description("Common information for raster-based images.");
		clazz.baseOn<Image>();

		TSdlEnumField<OwnerType, EImageSampleMode> sampleMode(&OwnerType::m_sampleMode);
		sampleMode.description("Sample mode of the raster image.");
		sampleMode.defaultTo(EImageSampleMode::Bilinear);
		sampleMode.optional();
		clazz.addField(sampleMode);

		TSdlEnumField<OwnerType, EImageWrapMode> wrapMode("wrap-mode", &OwnerType::m_wrapMode);
		wrapMode.description("Wrap mode of the raster image.");
		wrapMode.defaultTo(EImageWrapMode::Repeat);
		wrapMode.optional();
		clazz.addField(wrapMode);

		TSdlEnumField<OwnerType, EImageWrapMode> verticalWrapMode("vertical-wrap-mode", &OwnerType::m_verticalWrapMode);
		verticalWrapMode.description(
			"Wrap mode of the raster image in the vertical direction. If this field is specified, the "
			"<wrap-mode> field is treated as the horizontal wrap mode.");
		verticalWrapMode.defaultTo(EImageWrapMode::Unspecified);
		verticalWrapMode.optional();
		clazz.addField(verticalWrapMode);

		return clazz;
	}
};

}// end namespace ph
