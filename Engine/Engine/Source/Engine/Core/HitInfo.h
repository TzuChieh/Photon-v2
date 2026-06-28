#pragma once

#include "Engine/Math/TVector3.h"
#include "Engine/Math/TOrthonormalBasis3.h"
#include "Engine/Math/math.h"
#include "Engine/Core/ECoordSys.h"

#include <Common/assertion.h>
#include <Common/primitive_type.h>

namespace ph
{

class Primitive;

/*! @brief General information for a ray-primitive intersection.
*/
class HitInfo final
{
public:
	HitInfo();

	void setAttributes(
		const math::Vector3R& pos,
		const math::Vector3R& geometryNormal);

	void setAttributes(
		const math::Vector3R& pos,
		const math::Vector3R& geometryNormal,
		const math::Vector3R& shadingNormal);

	void setAttributes(
		const math::Vector3R& pos,
		const math::Vector3R& geometryNormal,
		const math::Vector3R& shadingNormal,
		const math::Vector3R& shadingTangent);

	void setDerivatives(
		const math::Vector3R& dPdU,
		const math::Vector3R& dPdV,
		const math::Vector3R& dNdU,
		const math::Vector3R& dNdV);

	void computeBases();

	math::Vector3R getPos() const;
	math::Vector3R getGeometryNormal() const;
	math::Vector3R getShadingNormal() const;

	/*!
	@return Shading tangent if `hasShadingTangent() == true`; otherwise, the value is undefined.
	*/
	math::Vector3R getShadingTangent() const;

	math::Vector3R getdPdU() const;
	math::Vector3R getdPdV() const;
	math::Vector3R getdNdU() const;
	math::Vector3R getdNdV() const;
	const math::Basis3R& getGeometryBasis() const;
	const math::Basis3R& getShadingBasis() const;

	bool hasShadingNormal() const;
	bool hasShadingTangent() const;

private:
	math::Vector3R m_pos;

	math::Vector3R m_dPdU;
	math::Vector3R m_dPdV;
	math::Vector3R m_dNdU;
	math::Vector3R m_dNdV;

	math::Basis3R m_geometryBasis;
	math::Basis3R m_shadingBasis;

	math::Vector3R m_shadingTangent;
	uint32 m_hasShadingNormal : 1;
	uint32 m_hasShadingTangent : 1;

#if PH_DEBUG
	bool m_isBasesComputed{false};
#endif
};

// In-header Implementations:

inline math::Vector3R HitInfo::getPos() const
{
	return m_pos;
}

inline math::Vector3R HitInfo::getGeometryNormal() const
{
	return m_geometryBasis.getYAxis();
}

inline math::Vector3R HitInfo::getShadingNormal() const
{
	PH_ASSERT(hasShadingNormal());
	return m_shadingBasis.getYAxis();
}

inline math::Vector3R HitInfo::getShadingTangent() const
{
	PH_ASSERT(hasShadingTangent());
	return m_shadingTangent;
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
	PH_ASSERT_MSG(m_isBasesComputed, "Please call `computeBases()` first.");
	return m_geometryBasis;
}

inline const math::Basis3R& HitInfo::getShadingBasis() const
{
	PH_ASSERT_MSG(m_isBasesComputed, "Please call `computeBases()` first.");
	return m_shadingBasis;
}

inline bool HitInfo::hasShadingNormal() const
{
	return m_hasShadingNormal;
}

inline bool HitInfo::hasShadingTangent() const
{
	return m_hasShadingTangent;
}

inline void HitInfo::setAttributes(
	const math::Vector3R& pos,
	const math::Vector3R& geometryNormal)
{
	m_pos = pos;
	m_geometryBasis.setYAxis(geometryNormal);
	m_shadingBasis.setYAxis(geometryNormal);
	m_hasShadingNormal = false;
	m_hasShadingTangent = false;

#if PH_DEBUG
	m_isBasesComputed = false;
#endif
}

inline void HitInfo::setAttributes(
	const math::Vector3R& pos,
	const math::Vector3R& geometryNormal,
	const math::Vector3R& shadingNormal)
{
	m_pos = pos;
	m_geometryBasis.setYAxis(geometryNormal);
	m_shadingBasis.setYAxis(shadingNormal);
	m_hasShadingNormal = true;
	m_hasShadingTangent = false;

#if PH_DEBUG
	m_isBasesComputed = false;
#endif
}

inline void HitInfo::setAttributes(
	const math::Vector3R& pos,
	const math::Vector3R& geometryNormal,
	const math::Vector3R& shadingNormal,
	const math::Vector3R& shadingTangent)
{
	m_pos = pos;
	m_geometryBasis.setYAxis(geometryNormal);
	m_shadingBasis.setYAxis(shadingNormal);
	m_shadingTangent = shadingTangent;
	m_hasShadingNormal = true;
	m_hasShadingTangent = true;

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

}// end namespace ph
