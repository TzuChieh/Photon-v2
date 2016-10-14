#include "Matrix4f.h"
#include "Vector3f.h"
#include "Quaternion.h"

#include <cmath>

namespace ph
{

Matrix4f::Matrix4f()
{
	for(uint32 i = 0; i < 4; i++)
	{
		for(uint32 j = 0; j < 4; j++)
		{
			m[i][j] = 0;
		}
	}
}

Matrix4f& Matrix4f::initIdentity()
{
	m[0][0] = 1;	m[0][1] = 0;	m[0][2] = 0;	m[0][3] = 0;
	m[1][0] = 0;	m[1][1] = 1;	m[1][2] = 0;	m[1][3] = 0;
	m[2][0] = 0;	m[2][1] = 0;	m[2][2] = 1;	m[2][3] = 0;
	m[3][0] = 0;	m[3][1] = 0;	m[3][2] = 0;	m[3][3] = 1;

	return *this;
}

Matrix4f& Matrix4f::initTranslation(const float32 x, const float32 y, const float32 z)
{
	m[0][0] = 1;	m[0][1] = 0;	m[0][2] = 0;	m[0][3] = x;
	m[1][0] = 0;	m[1][1] = 1;	m[1][2] = 0;	m[1][3] = y;
	m[2][0] = 0;	m[2][1] = 0;	m[2][2] = 1;	m[2][3] = z;
	m[3][0] = 0;	m[3][1] = 0;	m[3][2] = 0;	m[3][3] = 1;

	return *this;
}

Matrix4f& Matrix4f::initTranslation(const Vector3f& value)
{
	return initTranslation(value.x, value.y, value.z);
}

Matrix4f& Matrix4f::initRotation(const Quaternion& rot)
{
	m[0][0] = 1.0f - 2.0f*(rot.y*rot.y + rot.z*rot.z);
	m[0][1] = 2.0f*(rot.x*rot.y - rot.z*rot.w);
	m[0][2] = 2.0f*(rot.y*rot.w + rot.x*rot.z);
	m[0][3] = 0;

	m[1][0] = 2.0f*(rot.x*rot.y + rot.z*rot.w);
	m[1][1] = 1.0f - 2.0f*(rot.x*rot.x + rot.z*rot.z);
	m[1][2] = 2.0f*(rot.y*rot.z - rot.x*rot.w);
	m[1][3] = 0;

	m[2][0] = 2.0f*(rot.x*rot.z - rot.y*rot.w);
	m[2][1] = 2.0f*(rot.y*rot.z + rot.x*rot.w);
	m[2][2] = 1.0f - 2.0f*(rot.x*rot.x + rot.y*rot.y);
	m[2][3] = 0;

	m[3][0] = 0;
	m[3][1] = 0;
	m[3][2] = 0;
	m[3][3] = 1;

	return *this;
}

Matrix4f& Matrix4f::initScale(const float32 x, const float32 y, const float32 z)
{
	m[0][0] = x;	m[0][1] = 0;	m[0][2] = 0;	m[0][3] = 0;
	m[1][0] = 0;	m[1][1] = y;	m[1][2] = 0;	m[1][3] = 0;
	m[2][0] = 0;	m[2][1] = 0;	m[2][2] = z;	m[2][3] = 0;
	m[3][0] = 0;	m[3][1] = 0;	m[3][2] = 0;	m[3][3] = 1;

	return *this;
}

Matrix4f& Matrix4f::initScale(const Vector3f& scale)
{
	return initScale(scale.x, scale.y, scale.z);
}

Matrix4f& Matrix4f::initPerspectiveProjection(const float32 fov,
	const float32 aspectRatio,
	const float32 zNear,
	const float32 zFar)
{
	float32 tanHalfFov = tan(fov / 2.0f);
	float32 zRange = zNear - zFar;

	m[0][0] = 1.0f / tanHalfFov; m[0][1] = 0;                        m[0][2] = 0;                       m[0][3] = 0;
	m[1][0] = 0;                 m[1][1] = aspectRatio / tanHalfFov; m[1][2] = 0;                       m[1][3] = 0;
	m[2][0] = 0;                 m[2][1] = 0;                        m[2][2] = (zNear + zFar) / zRange;	m[2][3] = 2 * zFar * zNear / zRange;
	m[3][0] = 0;                 m[3][1] = 0;                        m[3][2] = -1.0f;	                m[3][3] = 0;

	return *this;
}

Matrix4f Matrix4f::mul(const Matrix4f& r)
{
	Matrix4f res;

	for(uint32 i = 0; i < 4; i++)
	{
		for(uint32 j = 0; j < 4; j++)
		{
			res.set(i, j, m[i][0] * r.get(0, j) +
			              m[i][1] * r.get(1, j) +
			              m[i][2] * r.get(2, j) +
			              m[i][3] * r.get(3, j));
		}
	}

	return res;
}

void Matrix4f::mul(const Matrix4f& r, Matrix4f* out_result) const
{
	for(uint32 i = 0; i < 4; i++)
	{
		for(uint32 j = 0; j < 4; j++)
		{
			out_result->m[i][j] = m[i][0] * r.m[0][j]
			                    + m[i][1] * r.m[1][j]
			                    + m[i][2] * r.m[2][j]
			                    + m[i][3] * r.m[3][j];
		}
	}
}

Matrix4f& Matrix4f::mulLocal(const float32 r)
{
	m[0][0] *= r;
	m[0][1] *= r;
	m[0][2] *= r;
	m[0][3] *= r;

	m[1][0] *= r;
	m[1][1] *= r;
	m[1][2] *= r;
	m[1][3] *= r;

	m[2][0] *= r;
	m[2][1] *= r;
	m[2][2] *= r;
	m[2][3] *= r;

	m[3][0] *= r;
	m[3][1] *= r;
	m[3][2] *= r;
	m[3][3] *= r;

	return *this;
}

Matrix4f& Matrix4f::inverse(Matrix4f* result) const
{
	result->m[0][0] = m[1][2] * m[2][3] * m[3][1] - m[1][3] * m[2][2] * m[3][1] + m[1][3] * m[2][1] * m[3][2]
	                - m[1][1] * m[2][3] * m[3][2] - m[1][2] * m[2][1] * m[3][3] + m[1][1] * m[2][2] * m[3][3];
	result->m[0][1] = m[0][3] * m[2][2] * m[3][1] - m[0][2] * m[2][3] * m[3][1] - m[0][3] * m[2][1] * m[3][2]
	                + m[0][1] * m[2][3] * m[3][2] + m[0][2] * m[2][1] * m[3][3] - m[0][1] * m[2][2] * m[3][3];
	result->m[0][2] = m[0][2] * m[1][3] * m[3][1] - m[0][3] * m[1][2] * m[3][1] + m[0][3] * m[1][1] * m[3][2]
	                - m[0][1] * m[1][3] * m[3][2] - m[0][2] * m[1][1] * m[3][3] + m[0][1] * m[1][2] * m[3][3];
	result->m[0][3] = m[0][3] * m[1][2] * m[2][1] - m[0][2] * m[1][3] * m[2][1] - m[0][3] * m[1][1] * m[2][2]
	                + m[0][1] * m[1][3] * m[2][2] + m[0][2] * m[1][1] * m[2][3] - m[0][1] * m[1][2] * m[2][3];

	result->m[1][0] = m[1][3] * m[2][2] * m[3][0] - m[1][2] * m[2][3] * m[3][0] - m[1][3] * m[2][0] * m[3][2]
	                + m[1][0] * m[2][3] * m[3][2] + m[1][2] * m[2][0] * m[3][3] - m[1][0] * m[2][2] * m[3][3];
	result->m[1][1] = m[0][2] * m[2][3] * m[3][0] - m[0][3] * m[2][2] * m[3][0] + m[0][3] * m[2][0] * m[3][2]
	                - m[0][0] * m[2][3] * m[3][2] - m[0][2] * m[2][0] * m[3][3] + m[0][0] * m[2][2] * m[3][3];
	result->m[1][2] = m[0][3] * m[1][2] * m[3][0] - m[0][2] * m[1][3] * m[3][0] - m[0][3] * m[1][0] * m[3][2]
	                + m[0][0] * m[1][3] * m[3][2] + m[0][2] * m[1][0] * m[3][3] - m[0][0] * m[1][2] * m[3][3];
	result->m[1][3] = m[0][2] * m[1][3] * m[2][0] - m[0][3] * m[1][2] * m[2][0] + m[0][3] * m[1][0] * m[2][2]
	                - m[0][0] * m[1][3] * m[2][2] - m[0][2] * m[1][0] * m[2][3] + m[0][0] * m[1][2] * m[2][3];

	result->m[2][0] = m[1][1] * m[2][3] * m[3][0] - m[1][3] * m[2][1] * m[3][0] + m[1][3] * m[2][0] * m[3][1]
	                - m[1][0] * m[2][3] * m[3][1] - m[1][1] * m[2][0] * m[3][3] + m[1][0] * m[2][1] * m[3][3];
	result->m[2][1] = m[0][3] * m[2][1] * m[3][0] - m[0][1] * m[2][3] * m[3][0] - m[0][3] * m[2][0] * m[3][1]
	                + m[0][0] * m[2][3] * m[3][1] + m[0][1] * m[2][0] * m[3][3] - m[0][0] * m[2][1] * m[3][3];
	result->m[2][2] = m[0][1] * m[1][3] * m[3][0] - m[0][3] * m[1][1] * m[3][0] + m[0][3] * m[1][0] * m[3][1]
	                - m[0][0] * m[1][3] * m[3][1] - m[0][1] * m[1][0] * m[3][3] + m[0][0] * m[1][1] * m[3][3];
	result->m[2][3] = m[0][3] * m[1][1] * m[2][0] - m[0][1] * m[1][3] * m[2][0] - m[0][3] * m[1][0] * m[2][1]
	                + m[0][0] * m[1][3] * m[2][1] + m[0][1] * m[1][0] * m[2][3] - m[0][0] * m[1][1] * m[2][3];

	result->m[3][0] = m[1][2] * m[2][1] * m[3][0] - m[1][1] * m[2][2] * m[3][0] - m[1][2] * m[2][0] * m[3][1]
	                + m[1][0] * m[2][2] * m[3][1] + m[1][1] * m[2][0] * m[3][2] - m[1][0] * m[2][1] * m[3][2];
	result->m[3][1] = m[0][1] * m[2][2] * m[3][0] - m[0][2] * m[2][1] * m[3][0] + m[0][2] * m[2][0] * m[3][1]
	                - m[0][0] * m[2][2] * m[3][1] - m[0][1] * m[2][0] * m[3][2] + m[0][0] * m[2][1] * m[3][2];
	result->m[3][2] = m[0][2] * m[1][1] * m[3][0] - m[0][1] * m[1][2] * m[3][0] - m[0][2] * m[1][0] * m[3][1]
	                + m[0][0] * m[1][2] * m[3][1] + m[0][1] * m[1][0] * m[3][2] - m[0][0] * m[1][1] * m[3][2];
	result->m[3][3] = m[0][1] * m[1][2] * m[2][0] - m[0][2] * m[1][1] * m[2][0] + m[0][2] * m[1][0] * m[2][1]
	                - m[0][0] * m[1][2] * m[2][1] - m[0][1] * m[1][0] * m[2][2] + m[0][0] * m[1][1] * m[2][2];

	return result->mulLocal(1.0f / determinant());
}

float32 Matrix4f::determinant() const
{
	float32 value;

	value = m[0][3] * m[1][2] * m[2][1] * m[3][0] - m[0][2] * m[1][3] * m[2][1] * m[3][0]
	      - m[0][3] * m[1][1] * m[2][2] * m[3][0] + m[0][1] * m[1][3] * m[2][2] * m[3][0]
	      + m[0][2] * m[1][1] * m[2][3] * m[3][0] - m[0][1] * m[1][2] * m[2][3] * m[3][0]
	      - m[0][3] * m[1][2] * m[2][0] * m[3][1] + m[0][2] * m[1][3] * m[2][0] * m[3][1]
	      + m[0][3] * m[1][0] * m[2][2] * m[3][1] - m[0][0] * m[1][3] * m[2][2] * m[3][1]
	      - m[0][2] * m[1][0] * m[2][3] * m[3][1] + m[0][0] * m[1][2] * m[2][3] * m[3][1]
	      + m[0][3] * m[1][1] * m[2][0] * m[3][2] - m[0][1] * m[1][3] * m[2][0] * m[3][2]
	      - m[0][3] * m[1][0] * m[2][1] * m[3][2] + m[0][0] * m[1][3] * m[2][1] * m[3][2]
          + m[0][1] * m[1][0] * m[2][3] * m[3][2] - m[0][0] * m[1][1] * m[2][3] * m[3][2]
	      - m[0][2] * m[1][1] * m[2][0] * m[3][3] + m[0][1] * m[1][2] * m[2][0] * m[3][3]
	      + m[0][2] * m[1][0] * m[2][1] * m[3][3] - m[0][0] * m[1][2] * m[2][1] * m[3][3]
	      - m[0][1] * m[1][0] * m[2][2] * m[3][3] + m[0][0] * m[1][1] * m[2][2] * m[3][3];

	return value;
}

std::string Matrix4f::toStringFormal() const
{
	std::string result;

	result += "[" + std::to_string(m[0][0]) + ", "
	              + std::to_string(m[0][1]) + ", "
	              + std::to_string(m[0][2]) + ", "
	              + std::to_string(m[0][3]) + "]\n";

	result += "[" + std::to_string(m[1][0]) + ", "
	              + std::to_string(m[1][1]) + ", "
	              + std::to_string(m[1][2]) + ", "
	              + std::to_string(m[1][3]) + "]\n";

	result += "[" + std::to_string(m[2][0]) + ", "
	              + std::to_string(m[2][1]) + ", "
	              + std::to_string(m[2][2]) + ", "
	              + std::to_string(m[2][3]) + "]\n";

	result += "[" + std::to_string(m[3][0]) + ", "
	              + std::to_string(m[3][1]) + ", "
	              + std::to_string(m[3][2]) + ", "
	              + std::to_string(m[3][3]) + "]";

	return result;
}

}// end namespace ph