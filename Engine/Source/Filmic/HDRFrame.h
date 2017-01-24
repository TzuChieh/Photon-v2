#pragma once

#include "Common/primitive_type.h"
#include "Filmic/Frame.h"

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
	virtual void getPixel(const uint32 x, const uint32 y, Vector3f* const out_pixel) const override;
	virtual void setPixel(const uint32 x, const uint32 y, const float32 r, const float32 g, const float32 b) override;

	virtual inline uint32 getWidthPx() const override
	{
		return m_widthPx;
	}

	virtual inline uint32 getHeightPx() const override
	{
		return m_heightPx;
	}

	virtual inline uint32 nPixelComponents() const override
	{
		return 3;
	}

	virtual inline const float32* getPixelData() const override
	{
		return m_pixelData.data();
	}

	inline float32 getPixel(const uint32 x, const uint32 y) const
	{
		return m_pixelData[y * getWidthPx() + x];
	}

	HDRFrame& operator = (const HDRFrame& rhs);
	HDRFrame& operator = (HDRFrame&& rhs);

private:
	uint32 m_widthPx;
	uint32 m_heightPx;

	std::vector<float32> m_pixelData;
};

}// end namespace ph