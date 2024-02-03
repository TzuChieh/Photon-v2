#pragma once

#include "Math/TVector3.h"
#include "Math/TOrthonormalBasis3.h"
#include "Math/math.h"
#include "Core/ECoordSys.h"
#include "Core/HitInfo.h"
#include "Core/FaceTopology.h"
#include "Utility/utility.h"

#include <Common/assertion.h>
#include <Common/primitive_type.h>

#include <cstddef>

namespace ph { class Primitive; }

namespace ph
{

/*! @brief Detailed information regarding a ray-primitive intersection.
*/
class HitDetail final
{
public:
	inline static constexpr auto NO_FACE_ID = static_cast<uint64>(-1);

	HitDetail();

	/*! @brief Set essential attributes that are independent to the coordinate system.
	*/
	HitDetail& setHitIntrinsics(
		const Primitive*      primitive,
		const math::Vector3R& uvw,
		real                  rayT,
		uint64                faceId = NO_FACE_ID,
		FaceTopology          faceTopology = FaceTopology(EFaceTopology::General));

	void computeBases();

	math::Vector3R getPosition(ECoordSys coordSys = ECoordSys::World) const;
	math::Vector3R getShadingNormal(ECoordSys coordSys = ECoordSys::World) const;
	math::Vector3R getGeometryNormal(ECoordSys coordSys = ECoordSys::World) const;
	math::Vector3R getdPdU(ECoordSys coordSys = ECoordSys::World) const;
	math::Vector3R getdPdV(ECoordSys coordSys = ECoordSys::World) const;
	math::Vector3R getdNdU(ECoordSys coordSys = ECoordSys::World) const;
	math::Vector3R getdNdV(ECoordSys coordSys = ECoordSys::World) const;
	math::Vector3R getUVW() const;
	const math::Basis3R& getGeometryBasis(ECoordSys coordSys = ECoordSys::World) const;
	const math::Basis3R& getShadingBasis(ECoordSys coordSys = ECoordSys::World) const;

	/*! @brief Get the parametric distance from the incident ray's origin.
	Notice that parametric distance is not ordinary distance but defined in terms of a ray
	direction vector's length.
	*/
	real getRayT() const;

	/*! @brief Get the face ID associated to the hit.
	May be `NO_FACE_ID` if not applicable to the hit.
	*/
	uint64 getFaceId() const;

	FaceTopology getFaceTopology() const;

	const Primitive* getPrimitive() const;
	const HitInfo& getHitInfo(ECoordSys coordSys = ECoordSys::World) const;
	HitInfo& getHitInfo(ECoordSys coordSys = ECoordSys::World);

private:
	const Primitive* m_primitive;
	math::Vector3R   m_uvw;
	real             m_rayT;
	HitInfo          m_hitInfos[enum_size<ECoordSys>()];
	uint64           m_faceId;
	FaceTopology     m_faceTopology;
};

// In-header Implementations:

inline math::Vector3R HitDetail::getPosition(const ECoordSys coordSys) const
{
	return getHitInfo(coordSys).getPosition();
}

inline math::Vector3R HitDetail::getShadingNormal(const ECoordSys coordSys) const
{
	return getHitInfo(coordSys).getShadingNormal();
}

inline math::Vector3R HitDetail::getGeometryNormal(const ECoordSys coordSys) const
{
	return getHitInfo(coordSys).getGeometryNormal();
}

inline math::Vector3R HitDetail::getdPdU(const ECoordSys coordSys) const
{
	return getHitInfo(coordSys).getdPdU();
}

inline math::Vector3R HitDetail::getdPdV(const ECoordSys coordSys) const
{
	return getHitInfo(coordSys).getdPdV();
}

inline math::Vector3R HitDetail::getdNdU(const ECoordSys coordSys) const
{
	return getHitInfo(coordSys).getdNdU();
}

inline math::Vector3R HitDetail::getdNdV(const ECoordSys coordSys) const
{
	return getHitInfo(coordSys).getdNdV();
}

inline const math::Basis3R& HitDetail::getGeometryBasis(const ECoordSys coordSys) const
{
	return getHitInfo(coordSys).getGeometryBasis();
}

inline const math::Basis3R& HitDetail::getShadingBasis(const ECoordSys coordSys) const
{
	return getHitInfo(coordSys).getShadingBasis();
}

inline real HitDetail::getRayT() const
{
	return m_rayT;
}

inline uint64 HitDetail::getFaceId() const
{
	return m_faceId;
}

inline FaceTopology HitDetail::getFaceTopology() const
{
	return FaceTopology(m_faceTopology);
}

inline const Primitive* HitDetail::getPrimitive() const
{
	return m_primitive;
}

inline math::Vector3R HitDetail::getUVW() const
{
	return m_uvw;
}

inline const HitInfo& HitDetail::getHitInfo(const ECoordSys coordSys) const
{
	PH_ASSERT_IN_RANGE(static_cast<int>(coordSys), 0, enum_size<ECoordSys>());

	return m_hitInfos[static_cast<int>(coordSys)];
}

inline HitInfo& HitDetail::getHitInfo(const ECoordSys coordSys)
{
	PH_ASSERT_IN_RANGE(static_cast<int>(coordSys), 0, enum_size<ECoordSys>());

	return m_hitInfos[static_cast<int>(coordSys)];
}

}// end namespace ph
