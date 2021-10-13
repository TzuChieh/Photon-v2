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
		return clazz;
	}
};

}// end namespace ph
