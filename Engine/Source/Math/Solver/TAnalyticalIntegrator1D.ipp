#include "Math/Solver/TAnalyticalIntegrator1D.h"
#include "Common/assertion.h"

#include <algorithm>

namespace ph::math
{

template<typename T>
TAnalyticalIntegrator1D<T>::TAnalyticalIntegrator1D() : 
	TAnalyticalIntegrator1D(0, 0)
{}
	
template<typename T>
TAnalyticalIntegrator1D<T>::TAnalyticalIntegrator1D(const T x0, const T x1) :
	m_x0(0), m_x1(0)
{
	setIntegrationDomain(x0, x1);
}

template<typename T>
T TAnalyticalIntegrator1D<T>::integrate(const TPiecewiseLinear1D<T>& func) const
{
	if(func.numPoints() == 0)
	{
		return 0;
	}

	const TVector2<T>& p0 = func.getPoint(0);
	const TVector2<T>& pN = func.getPoint(func.numPoints() - 1);

	// while there's only 1 point, or the integration domain 
	// does not intersect with [p0.x, pN.x]
	if(func.numPoints() == 1) return (m_x1 - m_x0) * p0.y;
	if(m_x1 < p0.x)           return (m_x1 - m_x0) * p0.y;
	if(m_x0 > pN.x)           return (m_x1 - m_x0) * pN.y;

	T sum = 0;

	// possibly add rectangular regions where x <= p0.x and x => pN.x
	if(m_x0 <= p0.x) sum += (p0.x - m_x0) * p0.y;
	if(m_x1 >= pN.x) sum += (m_x1 - pN.x) * pN.y;

	// possibly add trapezoidal regions where x > p0.x and x < pN.x
	for(std::size_t i = 0; i < func.numPoints() - 1; i++)
	{
		// intersecting integration domain with region's domain
		const auto& p0 = func.getPoint(i);
		const auto& p1 = func.getPoint(i + 1);
		const T x0 = std::max(m_x0, p0.x);
		const T x1 = std::min(m_x1, p1.x);

		// calculate the area of the trapezoid only if the intersection is valid
		if(x0 < x1)
		{
			sum += (func.evaluate(x0, i, i + 1) + func.evaluate(x1, i, i + 1)) * (x1 - x0) / 2;
		}
	}

	return sum;
}

template<typename T>
void TAnalyticalIntegrator1D<T>::setIntegrationDomain(const T x0, const T x1)
{
	PH_ASSERT(x1 >= x0);

	m_x0 = x0;
	m_x1 = x1;
}

}// end namespace ph::math
