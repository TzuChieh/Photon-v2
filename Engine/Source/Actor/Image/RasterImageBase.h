#pragma once

#include "Actor/Image/Image.h"
#include "Actor/Image/image_enums.h"
#include "Math/TVector2.h"
#include "Common/primitive_type.h"
#include "DataIO/SDL/sdl_interface.h"

namespace ph
{

class RasterImageBase : public Image
{
public:
	RasterImageBase();

	math::TVector2<uint32> getResolution() const;
	EImageSampleMode getSampleMode() const;
	EImageWrapMode getWrapMode() const;
	RasterImageBase& setSampleMode(EImageSampleMode mode);
	RasterImageBase& setWrapMode(EImageWrapMode mode);

protected:
	RasterImageBase& setResolution(math::TVector2<uint32> resolution);

private:
	math::TVector2<uint32> m_resolution;
	EImageSampleMode       m_sampleMode;
	EImageWrapMode         m_wrapMode;

public:
	PH_DEFINE_SDL_CLASS(TOwnerSdlClass<RasterImageBase>)
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

		TSdlEnumField<OwnerType, EImageWrapMode> wrapMode(&OwnerType::m_wrapMode);
		wrapMode.description("Wrap mode of the raster image.");
		wrapMode.defaultTo(EImageWrapMode::Repeat);
		wrapMode.optional();
		clazz.addField(wrapMode);

		return clazz;
	}
};

}// end namespace ph
