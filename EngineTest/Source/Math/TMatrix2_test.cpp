#include <Math/TMatrix2.h>
#include <Math/TVector2.h>
#include <Math/TVector3.h>

#include <gtest/gtest.h>

#include <type_traits>
#include <array>

using namespace ph;
using namespace ph::math;

TEST(TMatrix2Test, Requirements)
{
	EXPECT_TRUE(std::is_trivially_copyable_v<Matrix2R>);
	EXPECT_TRUE(std::is_trivially_copyable_v<Matrix2F>);
	EXPECT_TRUE(std::is_trivially_copyable_v<Matrix2D>);
}

TEST(TMatrix2Test, InitAndCopy)
{
	Matrix2R mat1(3);
	EXPECT_EQ(mat1.m[0][0], 3); EXPECT_EQ(mat1.m[0][1], 3);
	EXPECT_EQ(mat1.m[1][0], 3); EXPECT_EQ(mat1.m[1][1], 3);

	Matrix2R mat2(1, 2, 
	              3, 4);
	EXPECT_EQ(mat2.m[0][0], 1); EXPECT_EQ(mat2.m[0][1], 2);
	EXPECT_EQ(mat2.m[1][0], 3); EXPECT_EQ(mat2.m[1][1], 4);

	Matrix2R mat3 = mat2;
	EXPECT_EQ(mat3.m[0][0], 1); EXPECT_EQ(mat3.m[0][1], 2);
	EXPECT_EQ(mat3.m[1][0], 3); EXPECT_EQ(mat3.m[1][1], 4);
}

TEST(TMatrix2Test, Multiplying)
{
	TMatrix2<float> mat1(1, 2, 
	                     3, 4);
	TMatrix2<float> mat2(1, 1, 
	                     1, 1);
	TMatrix2<float> mat3 = mat1.mul(mat2);

	EXPECT_FLOAT_EQ(mat3.m[0][0], 3); EXPECT_FLOAT_EQ(mat3.m[0][1], 3);
	EXPECT_FLOAT_EQ(mat3.m[1][0], 7); EXPECT_FLOAT_EQ(mat3.m[1][1], 7);
}

TEST(TMatrix2Test, DeterminantAndInverse)
{
	TMatrix2<float> mat1(-1, -2,
	                      3,  4);
	EXPECT_FLOAT_EQ(mat1.determinant(), 2);

	TMatrix2<float> mat2(-0.1f, -0.2f,
	                      0.3f,  0.4f);
	EXPECT_FLOAT_EQ(mat2.determinant(), 0.02f);

	TMatrix2<float> mat3(1, 2,
	                     3, 4);
	TMatrix2<float> mat4 = mat3.inverse();
	EXPECT_FLOAT_EQ(mat4.m[0][0], -2.0f); EXPECT_FLOAT_EQ(mat4.m[0][1],  1.0f);
	EXPECT_FLOAT_EQ(mat4.m[1][0],  1.5f); EXPECT_FLOAT_EQ(mat4.m[1][1], -0.5f);
}

TEST(TMatrix2Test, SolveLinearSystem)
{
	// a solvable system (identity)
	{
		TMatrix2<float> A(
			1.0f, 0.0f,
			0.0f, 1.0f);
		TVector2<float> b(
			1.0f,
			1.0f);
		TVector2<float> x;
		ASSERT_TRUE(A.solve(b, &x));
		EXPECT_FLOAT_EQ(x.x, 1.0f);
		EXPECT_FLOAT_EQ(x.y, 1.0f);
	}

	// a solvable system
	{
		TMatrix2<float> A(
			0.1f, 0.2f,
			0.3f, 0.4f);
		TVector2<float> b(
			0.1f, 
			0.1f);
		TVector2<float> x;
		ASSERT_TRUE(A.solve(b, &x));
		EXPECT_FLOAT_EQ(x.x, -1);
		EXPECT_FLOAT_EQ(x.y,  1);
	}

	// a solvable system
	{
		TMatrix2<float> A(
			2.0f, 1.0f,
			1.0f, 2.0f);
		TVector2<float> b(
			3.0f, 
			3.0f);
		TVector2<float> x;
		ASSERT_TRUE(A.solve(b, &x));
		EXPECT_FLOAT_EQ(x.x, 1.0f);
		EXPECT_FLOAT_EQ(x.y, 1.0f);
	}

	// an unsolvable system (A is singular)
	{
		TMatrix2<float> A(
			7, 7,
			7, 7);
		TVector2<float> b(
			10,
			10);
		TVector2<float> x;
		ASSERT_FALSE(A.solve(b, &x));
	}

	// a solvable triple system
	{
		TMatrix2<float> A(
			1, 2,
			3, 4);
		std::array<std::array<float, 2>, 3> b = {
			1, 1,
			2, 2,
			3, 3};
		std::array<std::array<float, 2>, 3> x;
		ASSERT_TRUE(A.solve(b, &x));
		EXPECT_FLOAT_EQ(x[0][0], -1); EXPECT_FLOAT_EQ(x[0][1], 1);
		EXPECT_FLOAT_EQ(x[1][0], -2); EXPECT_FLOAT_EQ(x[1][1], 2);
		EXPECT_FLOAT_EQ(x[2][0], -3); EXPECT_FLOAT_EQ(x[2][1], 3);
	}

	// an unsolvable triple system (A is singular)
	{
		TMatrix2<float> A(
			2, 2,
			2, 2);
		std::array<std::array<float, 2>, 3> b = {
			1, 1,
			2, 2,
			3, 3};
		std::array<std::array<float, 2>, 3> x;
		ASSERT_FALSE(A.solve(b, &x));
	}
}
