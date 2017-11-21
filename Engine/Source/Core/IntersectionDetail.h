#pragma once

#include "Math/TVector3.h"
#include "Math/TOrthonormalBasis3.h"

namespace ph
{

class Primitive;

class IntersectionDetail final
{
public:
	IntersectionDetail();

	inline const Vector3R&  getPosition() const       { return m_position;            }
	inline const Vector3R&  getShadingNormal() const  { return m_shadingBasis.yAxis;  }
	inline const Vector3R&  getGeometryNormal() const { return m_geometryBasis.yAxis; }
	inline const Vector3R&  getUVW() const            { return m_uvw;                 }
	inline const Primitive* getPrimitive() const      { return m_primitive;           }
	inline real             getRayT() const           { return m_rayT;                }
	inline const Vector3R&  getdPdU() const           { return m_dPdU;                }
	inline const Vector3R&  getdPdV() const           { return m_dPdV;                }
	inline const Vector3R&  getdNdU() const           { return m_dNdU;                }
	inline const Vector3R&  getdNdV() const           { return m_dNdV;                }

	inline void setAttributes(
		const Primitive* primitive,
		const Vector3R&  position,
		const Vector3R&  geometryNormal,
		const Vector3R&  shadingNormal,
		const Vector3R&  uvw, 
		const real       rayT)
	{
		m_primitive           = primitive;
		m_position            = position;
		m_geometryBasis.yAxis = geometryNormal;
		m_shadingBasis.yAxis  = shadingNormal;
		m_uvw                 = uvw;
		m_rayT                = rayT;
	}

	inline void setDerivatives(
		const Vector3R& dPdU,
		const Vector3R& dPdV,
		const Vector3R& dNdU,
		const Vector3R& dNdV)
	{
		m_dPdU = dPdU;
		m_dPdV = dPdV;
		m_dNdU = dNdU;
		m_dNdV = dNdV;
	}

	inline void computeBases()
	{
		m_geometryBasis.xAxis = m_geometryBasis.yAxis.cross(m_dPdU).normalizeLocal();
		m_geometryBasis.zAxis = m_geometryBasis.xAxis.cross(m_geometryBasis.yAxis);

		m_shadingBasis.xAxis = m_shadingBasis.yAxis.cross(m_dNdU).normalizeLocal();
		m_shadingBasis.zAxis = m_shadingBasis.xAxis.cross(m_shadingBasis.yAxis);
	}

private:
	const Primitive* m_primitive;
	Vector3R         m_position;
	Vector3R         m_uvw;
	real             m_rayT;

	Vector3R m_dPdU;
	Vector3R m_dPdV;
	Vector3R m_dNdU;
	Vector3R m_dNdV;

	TOrthonormalBasis3<real> m_geometryBasis;
	TOrthonormalBasis3<real> m_shadingBasis;
};

}// end namespace ph