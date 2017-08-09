#pragma once

#include "Math/TVector2.h"

#include <vector>
#include <algorithm>
#include <functional>
#include <iostream>

namespace ph
{
	
// A class defining a piecewise linear function. Visually, points are
// connected by straight lines; thus the function value between point.x-es 
// are evaluated by linearly interpolating nearby point.y-es. Function value
// beyond the domain of specified point.x-es are evaluated with its nearest
// point.x's y value. If the function have got no points, evaluation results
// in zero no matter the input.
// 
// Notice that update() must be called before evaluating the function.

template<typename T>
class TPiecewiseLinear1D final
{
public:
	inline T evaluate(T x) const;
	inline T evaluate(T x, std::size_t p0Index, std::size_t p1Index) const;
	inline void update();
	inline void addPoint(const TVector2<T>& point);
	inline std::size_t numPoints() const;
	inline TVector2<T> getPoint(std::size_t pointIndex) const;

	inline std::string toString() const;

private:
	std::vector<TVector2<T>> m_points;

	static inline bool pointDomainComparator(const TVector2<T>& pA, const TVector2<T>& pB);
};

// implementations:

template<typename T>
inline T TPiecewiseLinear1D<T>::evaluate(const T x) const
{
	if(m_points.empty())
	{
		return 0;
	}

	// handling out-of-domain situation
	// (this also handles if there's only 1 unique point)
	if     (x <= m_points.front().x) return m_points.front().y;
	else if(x >= m_points.back().x)  return m_points.back().y;

	const auto& result = std::lower_bound(m_points.begin(), m_points.end(), TVector2<T>(x), 
	                                      &TPiecewiseLinear1D::pointDomainComparator);
	const std::size_t i1 = result - m_points.begin();
	const std::size_t i0 = i1 - 1;

	return evaluate(x, i0, i1);
}

template<typename T>
inline T TPiecewiseLinear1D<T>::evaluate(const T x, 
                                         const std::size_t p0Index, 
                                         const std::size_t p1Index) const
{
	const TVector2<T>& p0 = m_points[p0Index];
	const TVector2<T>& p1 = m_points[p1Index];
	return p0.x != p1.x ? (x - p0.x) / (p1.x - p0.x) * (p1.y - p0.y) + p0.y
	                    : (p0.y + p1.y) / 2;
}

template<typename T>
inline void TPiecewiseLinear1D<T>::update()
{
	std::stable_sort(m_points.begin(), m_points.end(), 
	                 &TPiecewiseLinear1D::pointDomainComparator);
}

template<typename T>
inline void TPiecewiseLinear1D<T>::addPoint(const TVector2<T>& point)
{
	m_points.push_back(point);
}

template<typename T>
inline std::size_t TPiecewiseLinear1D<T>::numPoints() const
{
	return m_points.size();
}

template<typename T>
inline TVector2<T> TPiecewiseLinear1D<T>::getPoint(const std::size_t pointIndex) const
{
	return m_points[pointIndex];
}

template<typename T>
inline std::string TPiecewiseLinear1D<T>::toString() const
{
	std::string result("piecewise linear function, composed of following points: ");
	for(const auto& point : m_points)
	{
		result += point.toString();
	}
	return result;
}

template<typename T>
inline bool TPiecewiseLinear1D<T>::pointDomainComparator(const TVector2<T>& pA, 
                                                         const TVector2<T>& pB)
{
	return pA.x < pB.x;
}

}// end namespace ph