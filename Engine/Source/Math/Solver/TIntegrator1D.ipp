#include "Math/Solver/TIntegrator1D.h"

namespace ph
{
	
template<typename T>
Integrator1D<T>::Integrator1D(const T x1, const T x2) :
	m_x1(x1), m_x2(x2)
{

}

template<typename T>
T Integrator1D<T>::integrate(const TPiecewiseLinear1D<T>& func) const
{
	// TODO
	return 0;
}

}// end namespace ph
