#include <Math/TMatrix2.h>
#include <Math/TVector2.h>
#include <Math/TVector3.h>

#include <gtest/gtest.h>

TEST(TMatrix2Test, InitAndCopy)
{
	using namespace ph;

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
	using namespace ph;

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
	using namespace ph;

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
	using namespace ph;

	// a solvable system

	TMatrix2<float> A1(0.1f, 0.2f,
	                   0.3f, 0.4f);
	TVector2<float> b1(0.1f, 
	                   0.1f);
	TVector2<float> x1;
	ASSERT_TRUE(A1.solve(b1, &x1));
	EXPECT_FLOAT_EQ(x1.x, -1);
	EXPECT_FLOAT_EQ(x1.y,  1);

	// an unsolvable system (A2 is singular)

	TMatrix2<float> A2(1, 1,
	                   1, 1);
	TVector2<float> b2(1, 
	                   1);
	TVector2<float> x2;
	ASSERT_FALSE(A2.solve(b2, &x2));

	// a solvable triple system

	TMatrix2<float> A3(1, 2,
	                   3, 4);
	TVector3<float> b3x(1, 2, 3);
	TVector3<float> b3y(1, 2, 3);
	TVector3<float> x3x;
	TVector3<float> x3y;
	ASSERT_TRUE(A3.solve(b3x, b3y, &x3x, &x3y));
	EXPECT_FLOAT_EQ(x3x.x, -1); EXPECT_FLOAT_EQ(x3x.y, -2); EXPECT_FLOAT_EQ(x3x.z, -3);
	EXPECT_FLOAT_EQ(x3y.x,  1); EXPECT_FLOAT_EQ(x3y.y,  2); EXPECT_FLOAT_EQ(x3y.z,  3);

	// an unsolvable triple system (A4 is singular)

	TMatrix2<float> A4(2, 2,
	                   2, 2);
	TVector3<float> b4x(1, 2, 3);
	TVector3<float> b4y(1, 2, 3);
	TVector3<float> x4x;
	TVector3<float> x4y;
	ASSERT_FALSE(A4.solve(b4x, b4y, &x4x, &x4y));
}