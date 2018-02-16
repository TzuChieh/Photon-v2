#pragma once

#include "Core/Ray.h"

namespace ph
{

class FullRay final
{
public:
	FullRay();
	FullRay(const Ray& headRay);

	inline const Ray& getHeadRay() const
	{
		return m_headRay;
	}

	inline void setHeadRay(const Ray& headRay)
	{
		m_headRay = headRay;
	}

private:
	Ray m_headRay;
};

}// end namespace ph