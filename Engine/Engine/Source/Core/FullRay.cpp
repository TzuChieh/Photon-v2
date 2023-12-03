#include "Core/FullRay.h"

namespace ph
{

FullRay::FullRay() : 
	FullRay(Ray())
{

}

FullRay::FullRay(const Ray& headRay) :
	m_headRay(headRay)
{

}

}// end namespace ph