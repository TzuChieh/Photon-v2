#pragma once

#include "Math/Function/TPiecewiseLinear1D.h"

namespace ph
{

template<typename T>
class Integrator1D final
{
public:
	Integrator1D(T x1, T x2);

	T integrate(const TPiecewiseLinear1D<T>& func) const;

private:
	T m_x1;
	T m_x2;
};

}// end namespace ph

#include "Math/Solver/TIntegrator1D.h"