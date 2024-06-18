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
#include <utility>
#include <cmath>

namespace ph { class Primitive; }

namespace ph
{

/*! @brief Detailed information regarding a ray-primitive intersection.
*/
class HitDetail final
{
public:
	inline static constexpr auto NO_FACE_ID = static_cast<uint64>(-1);

	/*! @brief Creates a hit detail filled with default values.
	*/
	HitDetail();

	/*! @brief Set essential attributes that are independent to the coordinate system.
	@param primitive The primitive that was hit.
	@param uvw The parametric coordinates of the hit.
	@param rayT The parametric distance of the hit relative to the ray. The unit of this
	value depends on the length of the ray's direction vector.
	@param faceID ID of the face that was hit. This ID may not be globally unique, see `getFaceID()`.
	@param faceTopology Surface topology locally around the hit point.
	*/
	HitDetail& setHitIntrinsics(
		const Primitive*      primitive,
		const math::Vector3R& uvw,
		real                  rayT,
		uint64                faceID = NO_FACE_ID,
		FaceTopology          faceTopology = FaceTopology(EFaceTopology::General));

	void computeBases();

	math::Vector3R getPos(ECoordSys coordSys = ECoordSys::World) const;
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
	@return The ID for the face that was hit. The ID is not globally unique--it is unique only
	for the primitive that was hit. For instanced triangle meshes, each mesh typically has the
	same set of face IDs. May be `NO_FACE_ID` if not available.
	*/
	uint64 getFaceID() const;

	/*!
	@return Surface topology locally around the hit point.
	*/
	FaceTopology getFaceTopology() const;

	/*!
	@return The primitive that was hit.
	*/
	const Primitive* getPrimitive() const;

	const HitInfo& getHitInfo(ECoordSys coordSys = ECoordSys::World) const;
	HitInfo& getHitInfo(ECoordSys coordSys = ECoordSys::World);
	std::pair<real, real> getDistanceErrorFactors() const;
	void setDistanceErrorFactors(real meanFactor, real maxFactor);

private:
	const Primitive* m_primitive;
	math::Vector3R   m_uvw;
	real             m_rayT;
	HitInfo          m_hitInfos[enum_size<ECoordSys>()];
	uint64           m_faceID;
	FaceTopology     m_faceTopology;
	int8			 m_meanDistanceErrorFactorExp2;
	int8			 m_maxDistanceErrorFactorExp2;
};

// In-header Implementations:

inline math::Vector3R HitDetail::getPos(const ECoordSys coordSys) const
{
	return getHitInfo(coordSys).getPos();
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

inline uint64 HitDetail::getFaceID() const
{
	return m_faceID;
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

inline std::pair<real, real> HitDetail::getDistanceErrorFactors() const
{
	return {
		static_cast<real>(std::exp2(m_meanDistanceErrorFactorExp2)),
		static_cast<real>(std::exp2(m_maxDistanceErrorFactorExp2))};
}

inline void HitDetail::setDistanceErrorFactors(const real meanFactor, const real maxFactor)
{
	// These should be absolute values
	PH_ASSERT_GE(meanFactor, 0.0_r);
	PH_ASSERT_GE(maxFactor, 0.0_r);

	m_meanDistanceErrorFactorExp2 = static_cast<int8>(std::round(std::log2(meanFactor)));
	m_maxDistanceErrorFactorExp2 = static_cast<int8>(std::ceil(std::log2(maxFactor)));
}

}// end namespace ph
