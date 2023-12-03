#include "Actor/Image/RasterImageBase.h"

namespace ph
{

math::TVector2<uint32> RasterImageBase::getResolution() const
{
	return m_resolution;
}

EImageSampleMode RasterImageBase::getSampleMode() const
{
	return m_sampleMode;
}

EImageWrapMode RasterImageBase::getHorizontalWrapMode() const
{
	return m_wrapMode;
}

EImageWrapMode RasterImageBase::getVerticalWrapMode() const
{
	return m_verticalWrapMode == EImageWrapMode::Unspecified ?
		m_wrapMode : m_verticalWrapMode;
}

RasterImageBase& RasterImageBase::setSampleMode(const EImageSampleMode mode)
{
	m_sampleMode = mode;
	return *this;
}

RasterImageBase& RasterImageBase::setWrapMode(const EImageWrapMode mode)
{
	m_wrapMode         = mode;
	m_verticalWrapMode = EImageWrapMode::Unspecified;
	return *this;
}

RasterImageBase& RasterImageBase::setWrapMode(const EImageWrapMode horizontalWrapMode, const EImageWrapMode verticalWrapMode)
{
	m_wrapMode         = horizontalWrapMode;
	m_verticalWrapMode = verticalWrapMode;
	return *this;
}

RasterImageBase& RasterImageBase::setResolution(const math::TVector2<uint32> resolution)
{
	m_resolution = resolution;
	return *this;
}

}// end namespace ph
