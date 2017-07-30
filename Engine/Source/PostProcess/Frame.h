#pragma once

#include "Common/primitive_type.h"
#include "Math/math_fwd.h"

#include <vector>

namespace ph
{

class Frame final
{
public:
	Frame();
	Frame(uint32 wPx, uint32 hPx);
	Frame(const Frame& other);
	Frame(Frame&& other);
	~Frame() = default;

	void getRgb(uint32 x, uint32 y, TVector3<float32>* out_rgb) const;
	void setRgb(uint32 x, uint32 y, const TVector3<float32>& rgb);
	const float32* getRgbData() const;

	Frame& operator = (Frame rhs);
	Frame& operator = (Frame&& rhs);

	inline uint32 widthPx() const
	{
		return m_widthPx;
	}

	inline uint32 heightPx() const
	{
		return m_heightPx;
	}

	inline std::size_t numRgbDataElements() const
	{
		return m_rgbData.size();
	}

	friend void swap(Frame& first, Frame& second);

private:
	uint32 m_widthPx;
	uint32 m_heightPx;

	std::vector<float32> m_rgbData;

	std::size_t calcRgbDataBaseIndex(uint32 x, uint32 y) const;
};

}// end namespace ph