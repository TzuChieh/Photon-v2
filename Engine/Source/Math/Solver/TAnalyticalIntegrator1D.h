#pragma once

#include "Math/Function/TPiecewiseLinear1D.h"

namespace ph::math
{

template<typename T>
class TAnalyticalIntegrator1D
{
public:
	TAnalyticalIntegrator1D();
	TAnalyticalIntegrator1D(T x0, T x1);

	T integrate(const TPiecewiseLinear1D<T>& func) const;

	void setIntegrationDomain(T x0, T x1);

private:
	T m_x0;
	T m_x1;
};

}// end namespace ph::math

#include "Math/Solver/TAnalyticalIntegrator1D.ipp"
