#include <Math/TMatrix3.h>

#include <gtest/gtest.h>

typedef ph::TMatrix3<float> Matrix;

TEST(TMatrix3Test, Construction)
{
	Matrix mat1;

	EXPECT_EQ(mat1.m[0][0], 0.0f);
	EXPECT_EQ(mat1.m[0][1], 0.0f);
	EXPECT_EQ(mat1.m[0][2], 0.0f);
	EXPECT_EQ(mat1.m[1][0], 0.0f);
	EXPECT_EQ(mat1.m[1][1], 0.0f);
	EXPECT_EQ(mat1.m[1][2], 0.0f);
	EXPECT_EQ(mat1.m[2][0], 0.0f);
	EXPECT_EQ(mat1.m[2][1], 0.0f);
	EXPECT_EQ(mat1.m[2][2], 0.0f);

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