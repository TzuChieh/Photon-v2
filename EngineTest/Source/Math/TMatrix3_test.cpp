#include <Math/TMatrix3.h>

#include <gtest/gtest.h>

#include <type_traits>

using namespace ph;
using namespace ph::math;

using Matrix = TMatrix3<float>;

namespace
{
	void expect_all_zero(const Matrix& matrix)
	{
		EXPECT_EQ(matrix.m[0][0], 0.0f);
		EXPECT_EQ(matrix.m[0][1], 0.0f);
		EXPECT_EQ(matrix.m[0][2], 0.0f);
		EXPECT_EQ(matrix.m[1][0], 0.0f);
		EXPECT_EQ(matrix.m[1][1], 0.0f);
		EXPECT_EQ(matrix.m[1][2], 0.0f);
		EXPECT_EQ(matrix.m[2][0], 0.0f);
		EXPECT_EQ(matrix.m[2][1], 0.0f);
		EXPECT_EQ(matrix.m[2][2], 0.0f);
	}

	void expect_identity(const Matrix& matrix)
	{
		EXPECT_EQ(matrix.m[0][0], 1.0f);
		EXPECT_EQ(matrix.m[0][1], 0.0f);
		EXPECT_EQ(matrix.m[0][2], 0.0f);
		EXPECT_EQ(matrix.m[1][0], 0.0f);
		EXPECT_EQ(matrix.m[1][1], 1.0f);
		EXPECT_EQ(matrix.m[1][2], 0.0f);
		EXPECT_EQ(matrix.m[2][0], 0.0f);
		EXPECT_EQ(matrix.m[2][1], 0.0f);
		EXPECT_EQ(matrix.m[2][2], 1.0f);
	}
}

TEST(TMatrix3Test, Requirements)
{
	using namespace ph;

	EXPECT_TRUE(std::is_trivially_copyable_v<Matrix3R>);
	EXPECT_TRUE(std::is_trivially_copyable_v<Matrix3F>);
	EXPECT_TRUE(std::is_trivially_copyable_v<Matrix3D>);
}

TEST(TMatrix3Test, Construction)
{
	Matrix mat1(0);

	expect_all_zero(mat1);

	Matrix mat2(Matrix::Elements{{
		{1.0f, 2.0f, 3.0f},
		{4.0f, 5.0f, 6.0f},
		{7.0f, 8.0f, 9.0f}
	}});

	EXPECT_EQ(mat2.m[0][0], 1.0f);
	EXPECT_EQ(mat2.m[0][1], 2.0f);
	EXPECT_EQ(mat2.m[0][2], 3.0f);
	EXPECT_EQ(mat2.m[1][0], 4.0f);
	EXPECT_EQ(mat2.m[1][1], 5.0f);
	EXPECT_EQ(mat2.m[1][2], 6.0f);
	EXPECT_EQ(mat2.m[2][0], 7.0f);
	EXPECT_EQ(mat2.m[2][1], 8.0f);
	EXPECT_EQ(mat2.m[2][2], 9.0f);
}

TEST(TMatrix3Test, Multiplying)
{
	Matrix mat1(Matrix::Elements{{
		{1.0f, 2.0f, 3.0f},
		{4.0f, 5.0f, 6.0f},
		{7.0f, 8.0f, 9.0f}
	}});

	Matrix mat2( 0.0f);
	Matrix mat3(-1.0f);

	Matrix mat4 = mat1.mul(mat2);
	Matrix mat5 = mat1.mul(mat3);

	expect_all_zero(mat4);

	EXPECT_EQ(mat5.m[0][0], -6.0f);
	EXPECT_EQ(mat5.m[0][1], -6.0f);
	EXPECT_EQ(mat5.m[0][2], -6.0f);
	EXPECT_EQ(mat5.m[1][0], -15.0f);
	EXPECT_EQ(mat5.m[1][1], -15.0f);
	EXPECT_EQ(mat5.m[1][2], -15.0f);
	EXPECT_EQ(mat5.m[2][0], -24.0f);
	EXPECT_EQ(mat5.m[2][1], -24.0f);
	EXPECT_EQ(mat5.m[2][2], -24.0f);
}

TEST(TMatrix3Test, CalcDeterminant)
{
	Matrix mat1(Matrix::Elements{{
		{1.0f, 2.0f, 3.0f},
		{4.0f, 5.0f, 6.0f},
		{7.0f, 8.0f, 9.0f}
	}});

	EXPECT_EQ(mat1.determinant(), 0.0f);

	Matrix mat2(Matrix::Elements{{
		{1.0f, 0.0f, 0.0f},
		{0.0f, 1.0f, 0.0f},
		{0.0f, 0.0f, 1.0f}
	}});
	
	EXPECT_EQ(mat2.determinant(), 1.0f);
}

TEST(TMatrix3Test, Inversing)
{
	Matrix mat1(Matrix::Elements{{
		{1.0f, 0.0f, 0.0f},
		{0.0f, 1.0f, 0.0f},
		{0.0f, 0.0f, 1.0f}
	}});

	expect_identity(mat1.inverse());

	Matrix mat2(Matrix::Elements{{
		{-1.0f,  2.0f,  3.0f},
		{ 1.0f, -2.0f,  3.0f},
		{ 1.0f,  2.0f, -3.0f}
	}});

	mat2 = mat2.inverse();

	EXPECT_EQ(mat2.m[0][0], 0.0f);
	EXPECT_EQ(mat2.m[0][1], 0.5f);
	EXPECT_EQ(mat2.m[0][2], 0.5f);
	EXPECT_EQ(mat2.m[1][0], 0.25f);
	EXPECT_EQ(mat2.m[1][1], 0.0f);
	EXPECT_EQ(mat2.m[1][2], 0.25f);
	EXPECT_EQ(mat2.m[2][0], 0.16666666666666666f);
	EXPECT_EQ(mat2.m[2][1], 0.16666666666666666f);
	EXPECT_EQ(mat2.m[2][2], 0.0f);
}
