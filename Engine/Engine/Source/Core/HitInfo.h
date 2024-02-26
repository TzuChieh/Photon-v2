#pragma once

#include "Math/TVector3.h"
#include "Math/TOrthonormalBasis3.h"
#include "Math/math.h"
#include "Core/ECoordSys.h"

#include <Common/assertion.h>

namespace ph
{

class Primitive;

class HitInfo final
{
public:
	HitInfo();

	void setAttributes(
		const math::Vector3R& position,
		const math::Vector3R& geometryNormal,
		const math::Vector3R& shadingNormal);

	void setDerivatives(
		const math::Vector3R& dPdU,
		const math::Vector3R& dPdV,
		const math::Vector3R& dNdU,
		const math::Vector3R& dNdV);

	void computeBases();

	math::Vector3R getPosition() const;
	math::Vector3R getShadingNormal() const;
	math::Vector3R getGeometryNormal() const;
	math::Vector3R getdPdU() const;
	math::Vector3R getdPdV() const;
	math::Vector3R getdNdU() const;
	math::Vector3R getdNdV() const;
	const math::Basis3R& getGeometryBasis() const;
	const math::Basis3R& getShadingBasis() const;

private:
	math::Vector3R m_position;

	math::Vector3R m_dPdU;
	math::Vector3R m_dPdV;
	math::Vector3R m_dNdU;
	math::Vector3R m_dNdV;

	math::Basis3R m_geometryBasis;
	math::Basis3R m_shadingBasis;

#if PH_DEBUG
	bool m_isBasesComputed{false};
#endif
};

// In-header Implementations:

inline math::Vector3R HitInfo::getPosition() const
{
	return m_position;
}

inline math::Vector3R HitInfo::getShadingNormal() const
{
	return m_shadingBasis.getYAxis();
}

inline math::Vector3R HitInfo::getGeometryNormal() const
{
	return m_geometryBasis.getYAxis();
}

inline math::Vector3R HitInfo::getdPdU() const
{
	return m_dPdU;
}

inline math::Vector3R HitInfo::getdPdV() const
{
	return m_dPdV;
}

inline math::Vector3R HitInfo::getdNdU() const
{
	return m_dNdU;
}

inline math::Vector3R HitInfo::getdNdV() const
{
	return m_dNdV;
}

inline const math::Basis3R& HitInfo::getGeometryBasis() const
{
	PH_ASSERT(m_isBasesComputed);
	return m_geometryBasis;
}

inline const math::Basis3R& HitInfo::getShadingBasis() const
{
	PH_ASSERT(m_isBasesComputed);
	return m_shadingBasis;
}

inline void HitInfo::setAttributes(
	const math::Vector3R& position,
	const math::Vector3R& geometryNormal,
	const math::Vector3R& shadingNormal)
{
	m_position = position;
	m_geometryBasis.setYAxis(geometryNormal);
	m_shadingBasis.setYAxis(shadingNormal);

#if PH_DEBUG
	m_isBasesComputed = false;
#endif
}

inline void HitInfo::setDerivatives(
	const math::Vector3R& dPdU,
	const math::Vector3R& dPdV,
	const math::Vector3R& dNdU,
	const math::Vector3R& dNdV)
{
	m_dPdU = dPdU;
	m_dPdV = dPdV;
	m_dNdU = dNdU;
	m_dNdV = dNdV;

#if PH_DEBUG
	m_isBasesComputed = false;
#endif
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
		m_geometryBasis = math::Basis3R::makeFromUnitY(m_geometryBasis.getYAxis());
	}

	m_shadingBasis.setXAxis(m_shadingBasis.getYAxis().cross(m_dNdU));
	if(m_shadingBasis.getXAxis().lengthSquared() > 0.0_r)
	{
		m_shadingBasis.renormalizeXAxis();
		m_shadingBasis.setZAxis(m_shadingBasis.getXAxis().cross(m_shadingBasis.getYAxis()));
	}
	else
	{
		m_shadingBasis = math::Basis3R::makeFromUnitY(m_shadingBasis.getYAxis());
	}

	PH_ASSERT_MSG(m_geometryBasis.getYAxis().isFinite() && m_shadingBasis.getYAxis().isFinite(), "\n"
		"m_geometryBasis.getYAxis() = " + m_geometryBasis.getYAxis().toString() + "\n"
		"m_shadingBasis.getYAxis()  = " + m_shadingBasis.getYAxis().toString() + "\n");

#if PH_DEBUG
	m_isBasesComputed = true;
#endif
}

}// end namespace ph
