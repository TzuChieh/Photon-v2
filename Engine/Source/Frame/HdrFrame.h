#pragma once

#include "Common/primitive_type.h"
#include "Frame/Frame.h"

#include <vector>

namespace ph
{

class HdrFrame : public Frame
{
public:
	HdrFrame(const uint32 widthPx, const uint32 heightPx);
	virtual ~HdrFrame() override;

	inline float32 getPixel(const uint32 x, const uint32 y) const
	{
		return m_pixelData[y * getWidthPx() + x];
	}

	inline void setPixel(const uint32 x, const uint32 y, const float32 pixel)
	{
		m_pixelData[y * getWidthPx() + x] = pixel;
	}

	inline const float32* getPixelData() const
	{
		return m_pixelData.data();
	}

private:
	std::vector<float32> m_pixelData;
};

}// end namespace ph