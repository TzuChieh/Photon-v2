#include <Math/TMatrix3.h>
#include <Math/TVector3.h>

#include <gtest/gtest.h>

#include <type_traits>

using namespace ph;
using namespace ph::math;

using Matrix = TMatrix3<float>;

namespace
{
	void expect_all_zero(const Matrix& matrix)
	{
		EXPECT_EQ(matrix[0][0], 0.0f);
		EXPECT_EQ(matrix[0][1], 0.0f);
		EXPECT_EQ(matrix[0][2], 0.0f);
		EXPECT_EQ(matrix[1][0], 0.0f);
		EXPECT_EQ(matrix[1][1], 0.0f);
		EXPECT_EQ(matrix[1][2], 0.0f);
		EXPECT_EQ(matrix[2][0], 0.0f);
		EXPECT_EQ(matrix[2][1], 0.0f);
		EXPECT_EQ(matrix[2][2], 0.0f);
	}

	void expect_identity(const Matrix& matrix)
	{
		EXPECT_EQ(matrix[0][0], 1.0f);
		EXPECT_EQ(matrix[0][1], 0.0f);
		EXPECT_EQ(matrix[0][2], 0.0f);
		EXPECT_EQ(matrix[1][0], 0.0f);
		EXPECT_EQ(matrix[1][1], 1.0f);
		EXPECT_EQ(matrix[1][2], 0.0f);
		EXPECT_EQ(matrix[2][0], 0.0f);
		EXPECT_EQ(matrix[2][1], 0.0f);
		EXPECT_EQ(matrix[2][2], 1.0f);
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

	EXPECT_EQ(mat2[0][0], 1.0f);
	EXPECT_EQ(mat2[0][1], 2.0f);
	EXPECT_EQ(mat2[0][2], 3.0f);
	EXPECT_EQ(mat2[1][0], 4.0f);
	EXPECT_EQ(mat2[1][1], 5.0f);
	EXPECT_EQ(mat2[1][2], 6.0f);
	EXPECT_EQ(mat2[2][0], 7.0f);
	EXPECT_EQ(mat2[2][1], 8.0f);
	EXPECT_EQ(mat2[2][2], 9.0f);
}

TEST(TMatrix3Test, Multiplying)
{
	// Naive matrix multiplication
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

		EXPECT_EQ(mat5[0][0], -6.0f);
		EXPECT_EQ(mat5[0][1], -6.0f);
		EXPECT_EQ(mat5[0][2], -6.0f);
		EXPECT_EQ(mat5[1][0], -15.0f);
		EXPECT_EQ(mat5[1][1], -15.0f);
		EXPECT_EQ(mat5[1][2], -15.0f);
		EXPECT_EQ(mat5[2][0], -24.0f);
		EXPECT_EQ(mat5[2][1], -24.0f);
		EXPECT_EQ(mat5[2][2], -24.0f);
	}

	// Integer matrix & matrix multiplication
	{
		const TMatrix3<int> matA(
			 1,  2,  3,
			 4,  5,  6,
			 7,  8,  9);

		const TMatrix3<int> matB(
			 3,  1, -5,
			 4,  1,  0,
			-2, -1,  8);

		const auto result = matA.mul(matB);

		EXPECT_EQ(result[0][0], 5);  EXPECT_EQ(result[0][1], 0); EXPECT_EQ(result[0][2], 19);
		EXPECT_EQ(result[1][0], 20); EXPECT_EQ(result[1][1], 3); EXPECT_EQ(result[1][2], 28);
		EXPECT_EQ(result[2][0], 35); EXPECT_EQ(result[2][1], 6); EXPECT_EQ(result[2][2], 37);
	}

	// Integer matrix & vector multiplication
	{
		const TMatrix3<int> mat(
			-1,  3,  7,
			 2, -2,  1,
			10,  0, -4);

		const TVector3<int> vec(
			 3,
			 4,
			-1);

		const auto result = mat * vec;

		EXPECT_EQ(result[0],  2);
		EXPECT_EQ(result[1], -3);
		EXPECT_EQ(result[2], 34);
	}
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

	EXPECT_EQ(mat2[0][0], 0.0f);
	EXPECT_EQ(mat2[0][1], 0.5f);
	EXPECT_EQ(mat2[0][2], 0.5f);
	EXPECT_EQ(mat2[1][0], 0.25f);
	EXPECT_EQ(mat2[1][1], 0.0f);
	EXPECT_EQ(mat2[1][2], 0.25f);
	EXPECT_EQ(mat2[2][0], 0.16666666666666666f);
	EXPECT_EQ(mat2[2][1], 0.16666666666666666f);
	EXPECT_EQ(mat2[2][2], 0.0f);
}
