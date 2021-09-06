#pragma once

#include "Math/TVector2.h"
#include "Common/assertion.h"

#include <vector>
#include <algorithm>
#include <functional>
#include <iostream>
#include <iterator>

namespace ph::math
{
	
// A class defining a piecewise linear function, where points (x, y) are
// connected by straight lines; function values between two nearby points 
// are evaluated by linearly interpolating their y-coordinates. Function 
// values beyond the domain of specified x coordinates are evaluated with 
// its nearest point's y value. If the function have got no points, 
// evaluation results in zero no matter the inputs.
// 
// Notice that update() must be called before feeding this function into 
// any other methods.
template<typename T>
class TPiecewiseLinear1D final
{
public:
	// Evaluates function value at <x>.
	T evaluate(T x) const;

	// Evaluates function value at <x> by linearly interpolating two points.
	// Note that the two points may not be neighbors.
	T evaluate(T x, std::size_t p0Index, std::size_t p1Index) const;

	// TODO: eliminate the need for update
	void update();
	
	void addPoint(const TVector2<T>& point);
	void addPoints(const TPiecewiseLinear1D& points);
	TPiecewiseLinear1D getMirrored(T pivotX) const;
	std::size_t numPoints() const;
	TVector2<T> getPoint(std::size_t pointIndex) const;
	std::string toString() const;

private:
	std::vector<TVector2<T>> m_points;

	static bool pointDomainComparator(const TVector2<T>& pA, const TVector2<T>& pB);
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
	if     (x <= m_points.front().x()) return m_points.front().y();
	else if(x >= m_points.back().x())  return m_points.back().y();

	const auto& result = std::lower_bound(m_points.begin(), m_points.end(), TVector2<T>(x), 
	                                      &TPiecewiseLinear1D::pointDomainComparator);
	PH_ASSERT(result != m_points.begin() && result != m_points.end());
	const std::size_t i1 = result - m_points.begin();
	const std::size_t i0 = i1 - 1;

	return evaluate(x, i0, i1);
}

template<typename T>
inline T TPiecewiseLinear1D<T>::evaluate(const T x, 
                                         const std::size_t p0Index, 
                                         const std::size_t p1Index) const
{
	PH_ASSERT(p0Index < m_points.size() && p1Index < m_points.size());

	const TVector2<T>& p0 = m_points[p0Index];
	const TVector2<T>& p1 = m_points[p1Index];
	return p0.x() != p1.x() ? (x - p0.x()) / (p1.x() - p0.x()) * (p1.y() - p0.y()) + p0.y()
	                        : (p0.y() + p1.y()) / 2;
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
inline void TPiecewiseLinear1D<T>::addPoints(const TPiecewiseLinear1D& points)
{
	m_points.insert(std::end(m_points), 
	                std::begin(points.m_points), std::end(points.m_points));
}

template<typename T>
inline TPiecewiseLinear1D<T> TPiecewiseLinear1D<T>::getMirrored(const T pivotX) const
{
	TPiecewiseLinear1D mirrored;
	for(const auto& point : m_points)
	{
		mirrored.addPoint({static_cast<T>(2) * pivotX - point.x(), point.y()});
	}
	mirrored.update();
	return mirrored;
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
	return pA.x() < pB.x();
}

}// end namespace ph::math
