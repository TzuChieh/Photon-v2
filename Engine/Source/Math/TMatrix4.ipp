#pragma once

#include "Math/TMatrix4.h"
#include "Math/TVector3.h"
#include "Math/TQuaternion.h"
#include "Common/assertion.h"

#include <cmath>

namespace ph
{

template<typename T>
TMatrix4<T> TMatrix4<T>::IDENTITY()
{
	return TMatrix4().initIdentity();
}

template<typename T>
inline TMatrix4<T>::TMatrix4() : 
	m{}
{}

template<typename T>
inline TMatrix4<T>::TMatrix4(const T value) : 
	m()
{
	for(std::size_t i = 0; i < 4; ++i)
	{
		for(std::size_t j = 0; j < 4; ++j)
		{
			m[i][j] = value;
		}
	}
}

template<typename T>
inline TMatrix4<T>::TMatrix4(const Elements& elements) :
	m(elements)
{}

template<typename T>
inline TMatrix4<T>::TMatrix4(const TMatrix4& other) : 
	m()
{
	for(std::size_t i = 0; i < 4; ++i)
	{
		for(std::size_t j = 0; j < 4; ++j)
		{
			m[i][j] = other.m[i][j];
		}
	}
}

template<typename T>
inline TMatrix4<T>::~TMatrix4() = default;

template<typename T>
template<typename U>
inline TMatrix4<T>::TMatrix4(const TMatrix4<U>& other) : 
	m()
{
	for(std::size_t i = 0; i < 4; ++i)
	{
		for(std::size_t j = 0; j < 4; ++j)
		{
			m[i][j] = static_cast<T>(other.m[i][j]);
		}
	}
}

template<typename T>
inline TMatrix4<T>& TMatrix4<T>::initIdentity()
{
	m[0][0] = 1;	m[0][1] = 0;	m[0][2] = 0;	m[0][3] = 0;
	m[1][0] = 0;	m[1][1] = 1;	m[1][2] = 0;	m[1][3] = 0;
	m[2][0] = 0;	m[2][1] = 0;	m[2][2] = 1;	m[2][3] = 0;
	m[3][0] = 0;	m[3][1] = 0;	m[3][2] = 0;	m[3][3] = 1;

	return *this;
}

template<typename T>
inline TMatrix4<T>& TMatrix4<T>::initTranslation(const T x, const T y, const T z)
{
	m[0][0] = 1;	m[0][1] = 0;	m[0][2] = 0;	m[0][3] = x;
	m[1][0] = 0;	m[1][1] = 1;	m[1][2] = 0;	m[1][3] = y;
	m[2][0] = 0;	m[2][1] = 0;	m[2][2] = 1;	m[2][3] = z;
	m[3][0] = 0;	m[3][1] = 0;	m[3][2] = 0;	m[3][3] = 1;

	return *this;
}

template<typename T>
inline TMatrix4<T>& TMatrix4<T>::initTranslation(const TVector3<T>& value)
{
	return initTranslation(value.x, value.y, value.z);
}

template<typename T>
inline TMatrix4<T>& TMatrix4<T>::initRotation(const TQuaternion<T>& rot)
{
	m[0][0] = 1 - 2*(rot.y*rot.y + rot.z*rot.z);
	m[0][1] = 2*(rot.x*rot.y - rot.z*rot.w);
	m[0][2] = 2*(rot.y*rot.w + rot.x*rot.z);
	m[0][3] = 0;

	m[1][0] = 2*(rot.x*rot.y + rot.z*rot.w);
	m[1][1] = 1 - 2*(rot.x*rot.x + rot.z*rot.z);
	m[1][2] = 2*(rot.y*rot.z - rot.x*rot.w);
	m[1][3] = 0;

	m[2][0] = 2*(rot.x*rot.z - rot.y*rot.w);
	m[2][1] = 2*(rot.y*rot.z + rot.x*rot.w);
	m[2][2] = 1 - 2*(rot.x*rot.x + rot.y*rot.y);
	m[2][3] = 0;

	m[3][0] = 0;
	m[3][1] = 0;
	m[3][2] = 0;
	m[3][3] = 1;

	return *this;
}

template<typename T>
inline TMatrix4<T>& TMatrix4<T>::initRotation(const TVector3<T>& orthBasisX, const TVector3<T>& orthBasisY, const TVector3<T>& orthBasisZ)
{
	m[0][0] = orthBasisX.x; m[0][1] = orthBasisX.y; m[0][2] = orthBasisX.z; m[0][3] = 0;
	m[1][0] = orthBasisY.x; m[1][1] = orthBasisY.y; m[1][2] = orthBasisY.z; m[1][3] = 0;
	m[2][0] = orthBasisZ.x; m[2][1] = orthBasisZ.y; m[2][2] = orthBasisZ.z; m[2][3] = 0;
	m[3][0] = 0;            m[3][1] = 0;            m[3][2] = 0;            m[3][3] = 1;

	return *this;
}

template<typename T>
inline TMatrix4<T>& TMatrix4<T>::initScale(const T x, const T y, const T z)
{
	m[0][0] = x; m[0][1] = 0; m[0][2] = 0; m[0][3] = 0;	
	m[1][0] = 0; m[1][1] = y; m[1][2] = 0; m[1][3] = 0;
	m[2][0] = 0; m[2][1] = 0; m[2][2] = z; m[2][3] = 0;
	m[3][0] = 0; m[3][1] = 0; m[3][2] = 0; m[3][3] = 1;

	return *this;
}

template<typename T>
inline TMatrix4<T>& TMatrix4<T>::initScale(const TVector3<T>& scale)
{
	return initScale(scale.x, scale.y, scale.z);
}

template<typename T>
inline TMatrix4<T>& TMatrix4<T>::initPerspectiveProjection(const T fov,
                                                           const T aspectRatio,
                                                           const T zNear,
                                                           const T zFar)
{
	const T tanHalfFov = std::tan(fov / 2);
	const T zRange     = zNear - zFar;

	m[0][0] = 1.0f / tanHalfFov; m[0][1] = 0;                        m[0][2] = 0;                       m[0][3] = 0;
	m[1][0] = 0;                 m[1][1] = aspectRatio / tanHalfFov; m[1][2] = 0;                       m[1][3] = 0;
	m[2][0] = 0;                 m[2][1] = 0;                        m[2][2] = (zNear + zFar) / zRange;	m[2][3] = 2 * zFar * zNear / zRange;
	m[3][0] = 0;                 m[3][1] = 0;                        m[3][2] = -1.0f;	                m[3][3] = 0;

	return *this;
}

template<typename T>
inline TMatrix4<T> TMatrix4<T>::mul(const TMatrix4& rhs) const
{
	TMatrix4 res;

	for(std::size_t i = 0; i < 4; ++i)
	{
		for(std::size_t j = 0; j < 4; ++j)
		{
			res.m[i][j] = m[i][0] * rhs.m[0][j] +
			              m[i][1] * rhs.m[1][j] +
			              m[i][2] * rhs.m[2][j] +
			              m[i][3] * rhs.m[3][j];
		}
	}

	return res;
}

template<typename T>
inline void TMatrix4<T>::mul(const TMatrix4& rhs, TMatrix4* const out_result) const
{
	PH_ASSERT(out_result != nullptr && out_result != this);

	for(std::size_t i = 0; i < 4; ++i)
	{
		for(std::size_t j = 0; j < 4; ++j)
		{
			out_result->m[i][j] = m[i][0] * rhs.m[0][j]
			                    + m[i][1] * rhs.m[1][j]
			                    + m[i][2] * rhs.m[2][j]
			                    + m[i][3] * rhs.m[3][j];
		}
	}
}

template<typename T>
inline void TMatrix4<T>::mul(const TVector3<T>& rhsXYZ, const T rhsW, TVector3<T>* const out_result) const
{
	PH_ASSERT(out_result != nullptr && out_result != &rhsXYZ);

	out_result->x = m[0][0] * rhsXYZ.x + m[0][1] * rhsXYZ.y + m[0][2] * rhsXYZ.z + m[0][3] * rhsW;
	out_result->y = m[1][0] * rhsXYZ.x + m[1][1] * rhsXYZ.y + m[1][2] * rhsXYZ.z + m[1][3] * rhsW;
	out_result->z = m[2][0] * rhsXYZ.x + m[2][1] * rhsXYZ.y + m[2][2] * rhsXYZ.z + m[2][3] * rhsW;
}

template<typename T>
inline TMatrix4<T>& TMatrix4<T>::mulLocal(const T rhs)
{
	m[0][0] *= rhs;
	m[0][1] *= rhs;
	m[0][2] *= rhs;
	m[0][3] *= rhs;

	m[1][0] *= rhs;
	m[1][1] *= rhs;
	m[1][2] *= rhs;
	m[1][3] *= rhs;

	m[2][0] *= rhs;
	m[2][1] *= rhs;
	m[2][2] *= rhs;
	m[2][3] *= rhs;

	m[3][0] *= rhs;
	m[3][1] *= rhs;
	m[3][2] *= rhs;
	m[3][3] *= rhs;

	return *this;
}

template<typename T>
inline TMatrix4<T>& TMatrix4<T>::inverse(TMatrix4* const out_result) const
{
	PH_ASSERT(out_result != nullptr && out_result != this);

	out_result->m[0][0] = m[1][2] * m[2][3] * m[3][1] - m[1][3] * m[2][2] * m[3][1] + m[1][3] * m[2][1] * m[3][2]
	                    - m[1][1] * m[2][3] * m[3][2] - m[1][2] * m[2][1] * m[3][3] + m[1][1] * m[2][2] * m[3][3];
	out_result->m[0][1] = m[0][3] * m[2][2] * m[3][1] - m[0][2] * m[2][3] * m[3][1] - m[0][3] * m[2][1] * m[3][2]
	                    + m[0][1] * m[2][3] * m[3][2] + m[0][2] * m[2][1] * m[3][3] - m[0][1] * m[2][2] * m[3][3];
	out_result->m[0][2] = m[0][2] * m[1][3] * m[3][1] - m[0][3] * m[1][2] * m[3][1] + m[0][3] * m[1][1] * m[3][2]
	                    - m[0][1] * m[1][3] * m[3][2] - m[0][2] * m[1][1] * m[3][3] + m[0][1] * m[1][2] * m[3][3];
	out_result->m[0][3] = m[0][3] * m[1][2] * m[2][1] - m[0][2] * m[1][3] * m[2][1] - m[0][3] * m[1][1] * m[2][2]
	                    + m[0][1] * m[1][3] * m[2][2] + m[0][2] * m[1][1] * m[2][3] - m[0][1] * m[1][2] * m[2][3];

	out_result->m[1][0] = m[1][3] * m[2][2] * m[3][0] - m[1][2] * m[2][3] * m[3][0] - m[1][3] * m[2][0] * m[3][2]
	                    + m[1][0] * m[2][3] * m[3][2] + m[1][2] * m[2][0] * m[3][3] - m[1][0] * m[2][2] * m[3][3];
	out_result->m[1][1] = m[0][2] * m[2][3] * m[3][0] - m[0][3] * m[2][2] * m[3][0] + m[0][3] * m[2][0] * m[3][2]
	                    - m[0][0] * m[2][3] * m[3][2] - m[0][2] * m[2][0] * m[3][3] + m[0][0] * m[2][2] * m[3][3];
	out_result->m[1][2] = m[0][3] * m[1][2] * m[3][0] - m[0][2] * m[1][3] * m[3][0] - m[0][3] * m[1][0] * m[3][2]
	                    + m[0][0] * m[1][3] * m[3][2] + m[0][2] * m[1][0] * m[3][3] - m[0][0] * m[1][2] * m[3][3];
	out_result->m[1][3] = m[0][2] * m[1][3] * m[2][0] - m[0][3] * m[1][2] * m[2][0] + m[0][3] * m[1][0] * m[2][2]
	                    - m[0][0] * m[1][3] * m[2][2] - m[0][2] * m[1][0] * m[2][3] + m[0][0] * m[1][2] * m[2][3];

	out_result->m[2][0] = m[1][1] * m[2][3] * m[3][0] - m[1][3] * m[2][1] * m[3][0] + m[1][3] * m[2][0] * m[3][1]
	                    - m[1][0] * m[2][3] * m[3][1] - m[1][1] * m[2][0] * m[3][3] + m[1][0] * m[2][1] * m[3][3];
	out_result->m[2][1] = m[0][3] * m[2][1] * m[3][0] - m[0][1] * m[2][3] * m[3][0] - m[0][3] * m[2][0] * m[3][1]
	                    + m[0][0] * m[2][3] * m[3][1] + m[0][1] * m[2][0] * m[3][3] - m[0][0] * m[2][1] * m[3][3];
	out_result->m[2][2] = m[0][1] * m[1][3] * m[3][0] - m[0][3] * m[1][1] * m[3][0] + m[0][3] * m[1][0] * m[3][1]
	                    - m[0][0] * m[1][3] * m[3][1] - m[0][1] * m[1][0] * m[3][3] + m[0][0] * m[1][1] * m[3][3];
	out_result->m[2][3] = m[0][3] * m[1][1] * m[2][0] - m[0][1] * m[1][3] * m[2][0] - m[0][3] * m[1][0] * m[2][1]
	                    + m[0][0] * m[1][3] * m[2][1] + m[0][1] * m[1][0] * m[2][3] - m[0][0] * m[1][1] * m[2][3];

	out_result->m[3][0] = m[1][2] * m[2][1] * m[3][0] - m[1][1] * m[2][2] * m[3][0] - m[1][2] * m[2][0] * m[3][1]
	                    + m[1][0] * m[2][2] * m[3][1] + m[1][1] * m[2][0] * m[3][2] - m[1][0] * m[2][1] * m[3][2];
	out_result->m[3][1] = m[0][1] * m[2][2] * m[3][0] - m[0][2] * m[2][1] * m[3][0] + m[0][2] * m[2][0] * m[3][1]
	                    - m[0][0] * m[2][2] * m[3][1] - m[0][1] * m[2][0] * m[3][2] + m[0][0] * m[2][1] * m[3][2];
	out_result->m[3][2] = m[0][2] * m[1][1] * m[3][0] - m[0][1] * m[1][2] * m[3][0] - m[0][2] * m[1][0] * m[3][1]
	                    + m[0][0] * m[1][2] * m[3][1] + m[0][1] * m[1][0] * m[3][2] - m[0][0] * m[1][1] * m[3][2];
	out_result->m[3][3] = m[0][1] * m[1][2] * m[2][0] - m[0][2] * m[1][1] * m[2][0] + m[0][2] * m[1][0] * m[2][1]
	                    - m[0][0] * m[1][2] * m[2][1] - m[0][1] * m[1][0] * m[2][2] + m[0][0] * m[1][1] * m[2][2];

	return out_result->mulLocal(1 / determinant());
}

template<typename T>
inline TMatrix4<T> TMatrix4<T>::transpose() const
{
	TMatrix4 result;
	for(std::size_t i = 0; i < 4; ++i)
	{
		for(std::size_t j = 0; j < 4; ++j)
		{
			result.m[j][i] = m[i][j];
		}
	}

	return result;
}

template<typename T>
inline T TMatrix4<T>::determinant() const
{
	T value;

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

template<typename T>
inline bool TMatrix4<T>::equals(const TMatrix4& other) const
{
	// TODO: branchless?

	for(std::size_t i = 0; i < 4; ++i)
	{
		for(std::size_t j = 0; j < 4; ++j)
		{
			if(m[i][j] != other.m[i][j])
			{
				return false;
			}
		}
	}

	return true;
}

template<typename T>
inline std::string TMatrix4<T>::toString() const
{
	std::string result;

	result += '[' + std::to_string(m[0][0]) + ", "
	              + std::to_string(m[0][1]) + ", "
	              + std::to_string(m[0][2]) + ", "
	              + std::to_string(m[0][3]) + "]\n";

	result += '[' + std::to_string(m[1][0]) + ", "
	              + std::to_string(m[1][1]) + ", "
	              + std::to_string(m[1][2]) + ", "
	              + std::to_string(m[1][3]) + "]\n";

	result += '[' + std::to_string(m[2][0]) + ", "
	              + std::to_string(m[2][1]) + ", "
	              + std::to_string(m[2][2]) + ", "
	              + std::to_string(m[2][3]) + "]\n";

	result += '[' + std::to_string(m[3][0]) + ", "
	              + std::to_string(m[3][1]) + ", "
	              + std::to_string(m[3][2]) + ", "
	              + std::to_string(m[3][3]) + "]";

	return result;
}

}// end namespace ph