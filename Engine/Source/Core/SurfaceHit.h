#pragma once

#include "Core/HitDetail.h"

namespace ph
{

class SurfaceHit final
{
public:
	inline SurfaceHit(const HitDetail& detail) : 
		m_detail(detail)
	{

	}

	inline const HitDetail& getDetail() const
	{
		return m_detail;
	}

private:
	HitDetail m_detail;
};

}// end namespace ph