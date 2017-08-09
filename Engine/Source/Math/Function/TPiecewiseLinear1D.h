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
	inline T evaluate(T x);
	inline void update();
	inline void addPoint(const TVector2<T>& point);
	inline std::size_t numPoints() const;
	inline TVector2<T> getPoint(std::size_t pointIndex) const;

	inline std::string toString() const;

private:
	std::vector<TVector2<T>> m_points;
};

// implementations:

template<typename T>
inline T TPiecewiseLinear1D<T>::evaluate(const T x)
{
	if(m_points.empty())
	{
		return 0;
	}

	// this also handles if there's only 1 unique point
	if     (x <= m_points.front().x) return m_points.front().y;
	else if(x >= m_points.back().x)  return m_points.back().y;

	const auto& comparator = [](const TVector2<T>& pA,
	                            const TVector2<T>& pB) -> bool
	{
		return pA.x < pB.x;
	};

	const auto& result = std::lower_bound(m_points.begin(), m_points.end(), 
	                                      TVector2<T>(x), comparator);
	const std::size_t i1 = result - m_points.begin();
	const std::size_t i0 = i1 - 1;

	const TVector2<T>& p0 = m_points[i0];
	const TVector2<T>& p1 = m_points[i1];
	return p0.x != p1.x ? (x - p0.x) / (p1.x - p0.x) * (p1.y - p0.y) + p0.y
	                    : (p0.y + p1.y) / 2;
}

template<typename T>
inline void TPiecewiseLinear1D<T>::update()
{
	std::stable_sort(m_points.begin(), m_points.end(), [](const TVector2<T>& pA, 
	                                                      const TVector2<T>& pB) -> bool
	{
		return pA.x < pB.x;
	});
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

}// end namespace ph