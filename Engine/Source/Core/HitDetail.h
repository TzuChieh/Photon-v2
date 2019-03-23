#pragma once

#include "Math/TVector3.h"
#include "Math/TOrthonormalBasis3.h"
#include "Math/math.h"
#include "Core/ECoordSys.h"
#include "Core/HitInfo.h"
#include "Common/assertion.h"

namespace ph
{

class Primitive;

class HitDetail final
{
public:
	HitDetail();

	// TODO: consider renaming to setHitIntrinsic()
	HitDetail& setMisc(
		const Primitive* primitive,
		const Vector3R&  uvw,
		real             rayT);

	void computeBases();

	Vector3R getPosition(ECoordSys coordSys = ECoordSys::WORLD) const;
	Vector3R getShadingNormal(ECoordSys coordSys = ECoordSys::WORLD) const;
	Vector3R getGeometryNormal(ECoordSys coordSys = ECoordSys::WORLD) const;
	Vector3R getdPdU(ECoordSys coordSys = ECoordSys::WORLD) const;
	Vector3R getdPdV(ECoordSys coordSys = ECoordSys::WORLD) const;
	Vector3R getdNdU(ECoordSys coordSys = ECoordSys::WORLD) const;
	Vector3R getdNdV(ECoordSys coordSys = ECoordSys::WORLD) const;
	Vector3R getUvw() const;
	const Basis3R& getGeometryBasis(ECoordSys coordSys = ECoordSys::WORLD) const;
	const Basis3R& getShadingBasis(ECoordSys coordSys = ECoordSys::WORLD) const;

	// Gets the parametric distance from the incident ray's origin. Notice that
	// parametric distance is not ordinary distance but defined in terms of a 
	// ray direction vector's length.
	real getRayT() const;

	const Primitive* getPrimitive() const;
	const HitInfo& getHitInfo(ECoordSys coordSys = ECoordSys::WORLD) const;
	HitInfo& getHitInfo(ECoordSys coordSys = ECoordSys::WORLD);

private:
	const Primitive* m_primitive;
	Vector3R         m_uvw;
	real             m_rayT;
	HitInfo          m_hitInfos[static_cast<int>(ECoordSys::NUM_ELEMENTS)];
};

// In-header Implementations:

inline Vector3R HitDetail::getPosition(const ECoordSys coordSys) const
{
	return getHitInfo(coordSys).getPosition();
}

inline Vector3R HitDetail::getShadingNormal(const ECoordSys coordSys) const
{
	return getHitInfo(coordSys).getShadingNormal();
}

inline Vector3R HitDetail::getGeometryNormal(const ECoordSys coordSys) const
{
	return getHitInfo(coordSys).getGeometryNormal();
}

inline Vector3R HitDetail::getdPdU(const ECoordSys coordSys) const
{
	return getHitInfo(coordSys).getdPdU();
}

inline Vector3R HitDetail::getdPdV(const ECoordSys coordSys) const
{
	return getHitInfo(coordSys).getdPdV();
}

inline Vector3R HitDetail::getdNdU(const ECoordSys coordSys) const
{
	return getHitInfo(coordSys).getdNdU();
}

inline Vector3R HitDetail::getdNdV(const ECoordSys coordSys) const
{
	return getHitInfo(coordSys).getdNdV();
}

inline const Basis3R& HitDetail::getGeometryBasis(const ECoordSys coordSys) const
{
	return getHitInfo(coordSys).getGeometryBasis();
}

inline const Basis3R& HitDetail::getShadingBasis(const ECoordSys coordSys) const
{
	return getHitInfo(coordSys).getShadingBasis();
}

inline real HitDetail::getRayT() const
{
	return m_rayT;
}

inline const Primitive* HitDetail::getPrimitive() const
{
	return m_primitive;
}

inline Vector3R HitDetail::getUvw() const
{
	return m_uvw;
}

inline const HitInfo& HitDetail::getHitInfo(const ECoordSys coordSys) const
{
	PH_ASSERT_LT(static_cast<int>(coordSys), static_cast<int>(ECoordSys::NUM_ELEMENTS));

	return m_hitInfos[static_cast<int>(coordSys)];
}

inline HitInfo& HitDetail::getHitInfo(const ECoordSys coordSys)
{
	PH_ASSERT_LT(static_cast<int>(coordSys), static_cast<int>(ECoordSys::NUM_ELEMENTS));

	return m_hitInfos[static_cast<int>(coordSys)];
}

// TODO: consider renaming to setHitIntrinsic()
inline HitDetail& HitDetail::setMisc(
	const Primitive* const primitive,
	const Vector3R&        uvw,
	const real             rayT)
{
	m_primitive = primitive;
	m_uvw = uvw;
	m_rayT = rayT;

	return *this;
}

inline void HitDetail::computeBases()
{
	for(int i = 0; i < static_cast<int>(ECoordSys::NUM_ELEMENTS); i++)
	{
		m_hitInfos[i].computeBases();
	}
}

}// end namespace ph