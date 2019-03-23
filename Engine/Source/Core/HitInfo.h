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
	HitInfo();

	void setAttributes(
		const Vector3R& position,
		const Vector3R& geometryNormal,
		const Vector3R& shadingNormal);

	void setDerivatives(
		const Vector3R& dPdU,
		const Vector3R& dPdV,
		const Vector3R& dNdU,
		const Vector3R& dNdV);

	void computeBases();

	Vector3R       getPosition() const;
	Vector3R       getShadingNormal() const;
	Vector3R       getGeometryNormal() const;
	Vector3R       getdPdU() const;
	Vector3R       getdPdV() const;
	Vector3R       getdNdU() const;
	Vector3R       getdNdV() const;
	const Basis3R& getGeometryBasis() const;
	const Basis3R& getShadingBasis() const;

private:
	Vector3R m_position;

	Vector3R m_dPdU;
	Vector3R m_dPdV;
	Vector3R m_dNdU;
	Vector3R m_dNdV;

	// TODO: basis may be constructed on demand (will this be costly?)
	Basis3R m_geometryBasis;
	Basis3R m_shadingBasis;
};

// In-header Implementations:

inline Vector3R HitInfo::getPosition() const
{
	return m_position;
}

inline Vector3R HitInfo::getShadingNormal() const
{
	return m_shadingBasis.getYAxis();
}

inline Vector3R HitInfo::getGeometryNormal() const
{
	return m_geometryBasis.getYAxis();
}

inline Vector3R HitInfo::getdPdU() const
{
	return m_dPdU;
}

inline Vector3R HitInfo::getdPdV() const
{
	return m_dPdV;
}

inline Vector3R HitInfo::getdNdU() const
{
	return m_dNdU;
}

inline Vector3R HitInfo::getdNdV() const
{
	return m_dNdV;
}

inline const Basis3R& HitInfo::getGeometryBasis() const
{
	return m_geometryBasis;
}

inline const Basis3R& HitInfo::getShadingBasis() const
{
	return m_shadingBasis;
}

inline void HitInfo::setAttributes(
	const Vector3R& position,
	const Vector3R& geometryNormal,
	const Vector3R& shadingNormal)
{
	m_position = position;
	m_geometryBasis.setYAxis(geometryNormal);
	m_shadingBasis.setYAxis(shadingNormal);
}

inline void HitInfo::setDerivatives(
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

inline void HitInfo::computeBases()
{
	// FIXME: currently this is a hacky way to avoid crossing two parallel vectors
	// (this condition can rarely happen)
	// (which will result in 0-length vector and cause normalization to fail)

	m_geometryBasis.setXAxis(m_geometryBasis.getYAxis().cross(m_dPdU));
	if(m_geometryBasis.getXAxis().lengthSquared() > 0.0_r)
	{
		m_geometryBasis.renormalizeXAxis();
		m_geometryBasis.setZAxis(m_geometryBasis.getXAxis().cross(m_geometryBasis.getYAxis()));
	}
	else
	{
		m_geometryBasis = Basis3R::makeFromUnitY(m_geometryBasis.getYAxis());
	}

	m_shadingBasis.setXAxis(m_shadingBasis.getYAxis().cross(m_dNdU));
	if(m_shadingBasis.getXAxis().lengthSquared() > 0.0_r)
	{
		m_shadingBasis.renormalizeXAxis();
		m_shadingBasis.setZAxis(m_shadingBasis.getXAxis().cross(m_shadingBasis.getYAxis()));
	}
	else
	{
		m_shadingBasis = Basis3R::makeFromUnitY(m_shadingBasis.getYAxis());
	}

	PH_ASSERT_MSG(m_geometryBasis.getYAxis().isFinite() && m_shadingBasis.getYAxis().isFinite(), "\n"
		"geometry-y-axis = " + m_geometryBasis.getYAxis().toString() + "\n"
		"shading-y-axis  = " + m_shadingBasis.getYAxis().toString() + "\n");
}

}// end namespace ph