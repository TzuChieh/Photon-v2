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
	typedef TOrthonormalBasis3<real> Basis;

	HitDetail();

	inline const Vector3R& getPosition(const ECoordSys coordSys = ECoordSys::WORLD) const 
	{
		return getHitInfo(coordSys).getPosition();
	}

	inline const Vector3R& getShadingNormal(const ECoordSys coordSys = ECoordSys::WORLD) const
	{
		return getHitInfo(coordSys).getShadingNormal();
	}

	inline const Vector3R& getGeometryNormal(const ECoordSys coordSys = ECoordSys::WORLD) const
	{
		return getHitInfo(coordSys).getGeometryNormal();
	}

	inline const Vector3R& getdPdU(const ECoordSys coordSys = ECoordSys::WORLD) const
	{
		return getHitInfo(coordSys).getdPdU();
	}

	inline const Vector3R& getdPdV(const ECoordSys coordSys = ECoordSys::WORLD) const
	{
		return getHitInfo(coordSys).getdPdV();
	}

	inline const Vector3R& getdNdU(const ECoordSys coordSys = ECoordSys::WORLD) const
	{
		return getHitInfo(coordSys).getdNdU();
	}

	inline const Vector3R& getdNdV(const ECoordSys coordSys = ECoordSys::WORLD) const
	{
		return getHitInfo(coordSys).getdNdV();
	}

	inline const Basis& getGeometryBasis(const ECoordSys coordSys = ECoordSys::WORLD) const
	{
		return getHitInfo(coordSys).getGeometryBasis();
	}

	inline const Basis& getShadingBasis(const ECoordSys coordSys = ECoordSys::WORLD) const
	{
		return getHitInfo(coordSys).getShadingBasis();
	}

	// Gets the parametric distance from the incident ray's origin. Notice that
	// parametric distance is not ordinary distance but defined in terms of a 
	// ray direction vector's length.
	//
	inline real getRayT() const
	{
		return m_rayT;
	}

	inline const Primitive* getPrimitive() const { return m_primitive; }
	inline const Vector3R&  getUvw() const       { return m_uvw;       }

	inline const HitInfo& getHitInfo(const ECoordSys coordSys = ECoordSys::WORLD) const
	{
		PH_ASSERT_LT(static_cast<int>(coordSys), static_cast<int>(ECoordSys::NUM_ELEMENTS));

		return m_hitInfos[static_cast<int>(coordSys)];
	}

	inline HitInfo& getHitInfo(const ECoordSys coordSys = ECoordSys::WORLD)
	{
		PH_ASSERT_LT(static_cast<int>(coordSys), static_cast<int>(ECoordSys::NUM_ELEMENTS));

		return m_hitInfos[static_cast<int>(coordSys)];
	}

	// TODO: consider renaming to setHitIntrinsic()
	inline HitDetail& setMisc(
		const Primitive* const primitive,
		const Vector3R&        uvw,
		const real             rayT)
	{
		m_primitive = primitive;
		m_uvw       = uvw;
		m_rayT      = rayT;

		return *this;
	}

	inline void computeBases()
	{
		for(int i = 0; i < static_cast<int>(ECoordSys::NUM_ELEMENTS); i++)
		{
			m_hitInfos[i].computeBases();
		}
	}

private:
	const Primitive* m_primitive;
	Vector3R         m_uvw;
	real             m_rayT;
	HitInfo          m_hitInfos[static_cast<int>(ECoordSys::NUM_ELEMENTS)];
};

}// end namespace ph