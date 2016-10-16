#pragma once

#include "Common/primitive_type.h"

namespace ph
{

class Frame
{
public:
	Frame(const uint32 widthPx, const uint32 heightPx);
	virtual ~Frame() = 0;

	inline uint32 getWidthPx() const
	{
		return m_widthPx;
	}

	inline uint32 getHeightPx() const
	{
		return m_heightPx;
	}

private:
	const uint32 m_widthPx;
	const uint32 m_heightPx;
};

}// end namespace ph