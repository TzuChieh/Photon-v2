#pragma once

#include "Math/TVector3.h"
#include "Math/TOrthonormalBasis3.h"
#include "Math/math.h"
#include "Core/ECoordSys.h"
#include "Common/assertion.h"

namespace ph
{

class Primitive;

class HitInfo final
{
public:
	typedef TOrthonormalBasis3<real> Basis;

	HitInfo();

	inline const Vector3R& getPosition() const       { return m_position;            }
	inline const Vector3R& getShadingNormal() const  { return m_shadingBasis.yAxis;  }
	inline const Vector3R& getGeometryNormal() const { return m_geometryBasis.yAxis; }
	inline const Vector3R& getdPdU() const           { return m_dPdU;                }
	inline const Vector3R& getdPdV() const           { return m_dPdV;                }
	inline const Vector3R& getdNdU() const           { return m_dNdU;                }
	inline const Vector3R& getdNdV() const           { return m_dNdV;                }
	inline const Basis&    getGeometryBasis() const  { return m_geometryBasis;       }
	inline const Basis&    getShadingBasis() const   { return m_shadingBasis;        }

	inline void setAttributes(
		const Vector3R& position,
		const Vector3R& geometryNormal,
		const Vector3R& shadingNormal)
	{
		m_position            = position;
		m_geometryBasis.yAxis = geometryNormal;
		m_shadingBasis.yAxis  = shadingNormal;
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
		if(m_dPdU.lengthSquared() > 0.0_r)
		{
			m_geometryBasis.xAxis = m_geometryBasis.yAxis.cross(m_dPdU).normalizeLocal();
			m_geometryBasis.zAxis = m_geometryBasis.xAxis.cross(m_geometryBasis.yAxis);
		}
		else
		{
			math::form_orthonormal_basis(m_geometryBasis.yAxis,
			                             &m_geometryBasis.xAxis, &m_geometryBasis.zAxis);
		}
		
		if(m_dNdU.lengthSquared() > 0.0_r)
		{
			m_shadingBasis.xAxis = m_shadingBasis.yAxis.cross(m_dNdU).normalizeLocal();
			m_shadingBasis.zAxis = m_shadingBasis.xAxis.cross(m_shadingBasis.yAxis);
		}
		else
		{
			math::form_orthonormal_basis(m_shadingBasis.yAxis,
			                             &m_shadingBasis.xAxis, &m_shadingBasis.zAxis);
		}

		PH_ASSERT_MSG(m_geometryBasis.yAxis.isFinite() && m_shadingBasis.yAxis.isFinite(), "\n"
			"geometry-y-axis = " + m_geometryBasis.yAxis.toString() + "\n"
			"shading-y-axis  = " + m_shadingBasis.yAxis.toString() + "\n");
	}

private:
	Vector3R m_position;

	Vector3R m_dPdU;
	Vector3R m_dPdV;
	Vector3R m_dNdU;
	Vector3R m_dNdV;

	// TODO: basis may be constructed on demand (will this be costly?)
	Basis m_geometryBasis;
	Basis m_shadingBasis;
};

}// end namespace ph