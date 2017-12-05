#pragma once

#include "Math/TVector3.h"
#include "Math/TOrthonormalBasis3.h"
#include "Math/Math.h"
#include "Core/ECoordSys.h"
#include "Core/HitInfo.h"

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

	inline const Vector3R& getUVW(const ECoordSys coordSys = ECoordSys::WORLD) const
	{
		return getHitInfo(coordSys).getUVW();
	}

	inline real getRayT(const ECoordSys coordSys = ECoordSys::WORLD) const
	{
		return getHitInfo(coordSys).getRayT();
	}

	inline const Primitive* getPrimitive() const
	{
		return m_primitive;
	}

	inline const HitInfo& getHitInfo(const ECoordSys coordSys = ECoordSys::WORLD) const
	{
		return m_hitInfos[static_cast<int>(coordSys)];
	}

	inline HitInfo& getHitInfo(const ECoordSys coordSys = ECoordSys::WORLD)
	{
		return m_hitInfos[static_cast<int>(coordSys)];
	}

	inline void setAdditional(const Primitive* const primitive)
	{
		m_primitive = primitive;
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
	HitInfo          m_hitInfos[static_cast<int>(ECoordSys::NUM_ELEMENTS)];
};

}// end namespace ph