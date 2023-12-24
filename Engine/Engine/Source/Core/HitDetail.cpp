#include "Core/HitDetail.h"

#include <limits>
#include <type_traits>

namespace ph
{

// A simple value type should be trivially copyable
static_assert(std::is_trivially_copyable_v<HitDetail>);

HitDetail::HitDetail() :
	m_primitive(nullptr),
	m_uvw      (0, 0, 0),
	m_rayT     (std::numeric_limits<real>::max()),
	m_faceId   (NO_FACE_ID),
	m_hitInfos {HitInfo(), HitInfo()}
{}

HitDetail& HitDetail::setHitIntrinsics(
	const Primitive* const primitive,
	const math::Vector3R&  uvw,
	const real             rayT,
	const std::size_t      faceId)
{
	m_primitive = primitive;
	m_uvw       = uvw;
	m_rayT      = rayT;
	m_faceId    = faceId;

	return *this;
}

void HitDetail::computeBases()
{
	for(int i = 0; i < static_cast<int>(ECoordSys::NUM_ELEMENTS); i++)
	{
		m_hitInfos[i].computeBases();
	}
}

}// end namespace ph
