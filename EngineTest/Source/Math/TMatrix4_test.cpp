#include <Math/TMatrix4.h>

#include <gtest/gtest.h>

#include <type_traits>

using namespace ph;
using namespace ph::math;

using Matrix = TMatrix4<float>;

namespace
{
	void expect_all_zero(const Matrix& matrix)
	{
		for(std::size_t i = 0; i < 4; ++i)
		{
			for(std::size_t j = 0; j < 4; ++j)
			{
				EXPECT_EQ(matrix.m[i][j], 0.0f);
			}
		}
	}

	void expect_identity(const Matrix& matrix)
	{
		for(std::size_t i = 0; i < 4; ++i)
		{
			for(std::size_t j = 0; j < 4; ++j)
			{
				if(i == j)
				{
					EXPECT_EQ(matrix.m[i][j], 1.0f);
				}
				else
				{
					EXPECT_EQ(matrix.m[i][j], 0.0f);
				}
			}
		}
	}
}

TEST(TMatrix4Test, Requirements)
{
	EXPECT_TRUE(std::is_trivially_copyable_v<Matrix4R>);
	EXPECT_TRUE(std::is_trivially_copyable_v<Matrix4F>);
	EXPECT_TRUE(std::is_trivially_copyable_v<Matrix4D>);
}

TEST(TMatrix4Test, Constructs)
{
	Matrix mat1(0);

	expect_all_zero(mat1);

	Matrix mat2(Matrix::Elements{{
		{1.0f, 2.0f, 3.0f, 4.0f},
		{5.0f, 6.0f, 7.0f, 8.0f},
		{9.0f, 10.0f, 11.0f, 12.0f},
		{13.0f, 14.0f, 15.0f, 16.0f}
	}});

	for(std::size_t i = 0; i < 4; ++i)
	{
		for(std::size_t j = 0; j < 4; ++j)
		{
			EXPECT_EQ(mat2.m[i][j], static_cast<float>(4 * i + j + 1));
		}
	}
}

TEST(TMatrix4Test, Multiplying)
{
	Matrix mat1(Matrix::Elements{{
		{1.0f, 2.0f, 3.0f, 4.0f},
		{5.0f, 6.0f, 7.0f, 8.0f},
		{9.0f, 10.0f, 11.0f, 12.0f},
		{13.0f, 14.0f, 15.0f, 16.0f}
	}});

	Matrix mat2(0.0f);
	Matrix mat3(1.0f);

	expect_all_zero(mat1.mul(mat2));

	mat3 = mat1.mul(mat3);

	EXPECT_EQ(mat3.m[0][0], 10.0f);
	EXPECT_EQ(mat3.m[0][1], 10.0f);
	EXPECT_EQ(mat3.m[0][2], 10.0f);
	EXPECT_EQ(mat3.m[0][3], 10.0f);
	EXPECT_EQ(mat3.m[1][0], 26.0f);
	EXPECT_EQ(mat3.m[1][1], 26.0f);
	EXPECT_EQ(mat3.m[1][2], 26.0f);
	EXPECT_EQ(mat3.m[1][3], 26.0f);
	EXPECT_EQ(mat3.m[2][0], 42.0f);
	EXPECT_EQ(mat3.m[2][1], 42.0f);
	EXPECT_EQ(mat3.m[2][2], 42.0f);
	EXPECT_EQ(mat3.m[2][3], 42.0f);
	EXPECT_EQ(mat3.m[3][0], 58.0f);
	EXPECT_EQ(mat3.m[3][1], 58.0f);
	EXPECT_EQ(mat3.m[3][2], 58.0f);
	EXPECT_EQ(mat3.m[3][3], 58.0f);
}

TEST(TMatrix4Test, CalcDeterminant)
{
	Matrix mat1(0.0f);

	EXPECT_EQ(mat1.determinant(), 0.0f);

	Matrix mat2(1.0f);

	EXPECT_EQ(mat2.determinant(), 0.0f);

	Matrix mat3;
	mat3.initIdentity();

	expect_identity(mat3);
	EXPECT_EQ(mat3.determinant(), 1.0f);

	Matrix mat4(Matrix::Elements{{
		{1.0f, 2.0f, 3.0f, 4.0f},
		{2.0f, 3.0f, 4.0f, 1.0f},
		{4.0f, 3.0f, 2.0f, 1.0f},
		{1.0f, 1.0f, 3.0f, 7.0f}
	}});

	EXPECT_EQ(mat4.determinant(), 40.0f);
}

TEST(TMatrix4Test, Inversing)
{
	Matrix mat1;
	mat1.initIdentity();

	expect_identity(mat1);

	Matrix inversedMat1;
	mat1.inverse(&inversedMat1);

	expect_identity(inversedMat1);

	Matrix mat2(Matrix::Elements{{
		{-1.0f,  2.0f,  3.0f,  4.0f},
		{ 1.0f, -2.0f,  3.0f,  4.0f},
		{ 1.0f,  2.0f, -3.0f,  4.0f},
		{ 1.0f,  2.0f,  3.0f, -4.0f}
	}});

	Matrix inversedMat2;
	mat2.inverse(&inversedMat2);

	EXPECT_EQ(inversedMat2.m[0][0], -0.25f);
	EXPECT_EQ(inversedMat2.m[0][1], 0.25f);
	EXPECT_EQ(inversedMat2.m[0][2], 0.25f);
	EXPECT_EQ(inversedMat2.m[0][3], 0.25f);
	EXPECT_EQ(inversedMat2.m[1][0], 0.125f);
	EXPECT_EQ(inversedMat2.m[1][1], -0.125f);
	EXPECT_EQ(inversedMat2.m[1][2], 0.125f);
	EXPECT_EQ(inversedMat2.m[1][3], 0.125f);
	EXPECT_EQ(inversedMat2.m[2][0], 0.08333333333333333f);
	EXPECT_EQ(inversedMat2.m[2][1], 0.08333333333333333f);
	EXPECT_EQ(inversedMat2.m[2][2], -0.08333333333333333f);
	EXPECT_EQ(inversedMat2.m[2][3], 0.08333333333333333f);
	EXPECT_EQ(inversedMat2.m[3][0], 0.0625f);
	EXPECT_EQ(inversedMat2.m[3][1], 0.0625f);
	EXPECT_EQ(inversedMat2.m[3][2], 0.0625f);
	EXPECT_EQ(inversedMat2.m[3][3], -0.0625f);
}

TEST(TMatrix4Test, Transposing)
{
	Matrix mat1;
	mat1.initIdentity();

	expect_identity(mat1);
	expect_identity(mat1.transpose());

	Matrix mat2(Matrix::Elements{{
		{1.0f, 2.0f, 3.0f, 4.0f},
		{5.0f, 6.0f, 7.0f, 8.0f},
		{9.0f, 10.0f, 11.0f, 12.0f},
		{13.0f, 14.0f, 15.0f, 16.0f}
	}});

	mat2 = mat2.transpose();

	EXPECT_EQ(mat2.m[0][0], 1.0f);
	EXPECT_EQ(mat2.m[0][1], 5.0f);
	EXPECT_EQ(mat2.m[0][2], 9.0f);
	EXPECT_EQ(mat2.m[0][3], 13.0f);
	EXPECT_EQ(mat2.m[1][0], 2.0f);
	EXPECT_EQ(mat2.m[1][1], 6.0f);
	EXPECT_EQ(mat2.m[1][2], 10.0f);
	EXPECT_EQ(mat2.m[1][3], 14.0f);
	EXPECT_EQ(mat2.m[2][0], 3.0f);
	EXPECT_EQ(mat2.m[2][1], 7.0f);
	EXPECT_EQ(mat2.m[2][2], 11.0f);
	EXPECT_EQ(mat2.m[2][3], 15.0f);
	EXPECT_EQ(mat2.m[3][0], 4.0f);
	EXPECT_EQ(mat2.m[3][1], 8.0f);
	EXPECT_EQ(mat2.m[3][2], 12.0f);
	EXPECT_EQ(mat2.m[3][3], 16.0f);
}

TEST(TMatrix4Test, Equality)
{
	Matrix mat1(Matrix::Elements{{
		{-1.0f,  2.0f,  3.0f,  4.0f},
		{ 1.0f, -2.0f,  3.0f,  4.0f},
		{ 1.0f,  2.0f, -3.0f,  4.0f},
		{ 1.0f,  2.0f,  3.0f, -4.0f}
	}});

	Matrix mat2(Matrix::Elements{{
		{-1.0f,  2.0f,  3.0f,  4.0f},
		{ 1.0f, -2.0f,  3.0f,  4.0f},
		{ 1.0f,  2.0f, -3.0f,  4.0f},
		{ 1.0f,  2.0f,  3.0f, -4.0f}
	}});

	EXPECT_TRUE(mat1.equals(mat2));
	EXPECT_FALSE(mat1.equals(Matrix(999.0f)));
}

TEST(TMatrix4Test, RotationMatrixFromBasis)
{
	Matrix mat1;
	mat1.initRotation({1, 0, 0}, {0, 1, 0}, {0, 0, 1});
	EXPECT_TRUE(mat1.equals(Matrix::IDENTITY()));

	Matrix mat2;
	mat2.initRotation({0, 0, 1}, {-1, 0, 0}, {0, -1, 0});

	Vector3R result;
	mat2.mul({1, 1, 1}, 0, &result);
	EXPECT_FLOAT_EQ(result.x, -1);
	EXPECT_FLOAT_EQ(result.y, -1);
	EXPECT_FLOAT_EQ(result.z,  1);
}
