#include "Actor/Image/RasterImageBase.h"

namespace ph
{

RasterImageBase::RasterImageBase() :

	Image(),

	m_resolution(0),
	m_sampleMode(EImageSampleMode::UNSPECIFIED),
	m_wrapMode  (EImageWrapMode::UNSPECIFIED)
{}

math::TVector2<uint32> RasterImageBase::getResolution() const
{
	return m_resolution;
}

EImageSampleMode RasterImageBase::getSampleMode() const
{
	return m_sampleMode;
}

EImageWrapMode RasterImageBase::getWrapMode() const
{
	return m_wrapMode;
}

RasterImageBase& RasterImageBase::setSampleMode(const EImageSampleMode mode)
{
	m_sampleMode = mode;

	return *this;
}

RasterImageBase& RasterImageBase::setWrapMode(const EImageWrapMode mode)
{
	m_wrapMode = mode;

	return *this;
}

RasterImageBase& RasterImageBase::setResolution(const math::TVector2<uint32> resolution)
{
	m_resolution = resolution;
}

}// end namespace ph
