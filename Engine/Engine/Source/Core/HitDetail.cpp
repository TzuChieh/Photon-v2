#include "Core/HitDetail.h"

#include <limits>
#include <type_traits>

namespace ph
{

// A simple value type should be trivially copyable
static_assert(std::is_trivially_copyable_v<HitDetail>);

HitDetail::HitDetail()
	: m_primitive                  (nullptr)
	, m_uvw                        (0, 0, 0)
	, m_rayT                       (std::numeric_limits<real>::max())
	, m_hitInfos                   {HitInfo{}, HitInfo{}}
	, m_faceID                     (NO_FACE_ID)
	, m_faceTopology               (EFaceTopology::General)
	, m_meanDistanceErrorFactorExp2(-64)// ~= 5.42*10^-20, small enough to fail fast
	, m_maxDistanceErrorFactorExp2 (-64)//
{}

HitDetail& HitDetail::setHitIntrinsics(
	const Primitive* const primitive,
	const math::Vector3R&  uvw,
	const real             rayT,
	const uint64           faceID,
	const FaceTopology     faceTopology)
{
	m_primitive    = primitive;
	m_uvw          = uvw;
	m_rayT         = rayT;
	m_faceID       = faceID;
	m_faceTopology = faceTopology;

	return *this;
}

void HitDetail::computeBases()
{
	for(HitInfo& hitInfo : m_hitInfos)
	{
		hitInfo.computeBases();
	}
}

}// end namespace ph
