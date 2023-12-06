#include <Math/Solver/TAnalyticalIntegrator1D.h>
#include <Math/Function/TPiecewiseLinear1D.h>
#include <Common/primitive_type.h>

#include <gtest/gtest.h>

using namespace ph;
using namespace ph::math;

TEST(MathSolverTest, AnalyticallyIntegratePiecewiseLinearFunction)
{
	TPiecewiseLinear1D<float32> func;
	func.addPoint(TVector2<float32>(-1, 1));
	func.addPoint(TVector2<float32>( 0, 2));
	func.addPoint(TVector2<float32>( 1, 1));
	func.update();

	TAnalyticalIntegrator1D<float32> integrator1(-2, -1);
	EXPECT_FLOAT_EQ(integrator1.integrate(func), 1.0f);

	TAnalyticalIntegrator1D<float32> integrator2(-1, 0);
	EXPECT_FLOAT_EQ(integrator2.integrate(func), 1.5f);

	TAnalyticalIntegrator1D<float32> integrator3(-1, 1);
	EXPECT_FLOAT_EQ(integrator3.integrate(func), 3.0f);

	TAnalyticalIntegrator1D<float32> integrator4(-2, 2);
	EXPECT_FLOAT_EQ(integrator4.integrate(func), 5.0f);
}
