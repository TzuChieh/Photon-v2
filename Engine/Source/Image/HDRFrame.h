#pragma once

#include "Common/primitive_type.h"
#include "Image/Frame.h"

#include <vector>

namespace ph
{

class HDRFrame : public Frame
{
public:
	HDRFrame();
	HDRFrame(const uint32 widthPx, const uint32 heightPx);
	HDRFrame(const HDRFrame& other);
	HDRFrame(HDRFrame&& other);
	virtual ~HDRFrame() override;

	virtual void resize(const uint32 newWidthPx, const uint32 newHeightPx) override;
	virtual void setPixel(const uint32 x, const uint32 y, const float32 r, const float32 g, const float32 b) override;

	virtual inline uint32 getWidthPx() const override
	{
		return m_widthPx;
	}

	virtual inline uint32 getHeightPx() const override
	{
		return m_heightPx;
	}

	inline float32 getPixel(const uint32 x, const uint32 y) const
	{
		return m_pixelData[y * getWidthPx() + x];
	}

	inline const float32* getPixelData() const
	{
		return m_pixelData.data();
	}

	HDRFrame& operator = (const HDRFrame& rhs);
	HDRFrame& operator = (HDRFrame&& rhs);

private:
	uint32 m_widthPx;
	uint32 m_heightPx;

	std::vector<float32> m_pixelData;
};

}// end namespace ph