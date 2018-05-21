#include "Core/HitDetail.h"

#include <limits>

namespace ph
{

HitDetail::HitDetail() :
	m_primitive(nullptr),
	m_uvw(0, 0, 0),
	m_rayT(std::numeric_limits<real>::infinity()),
	m_hitInfos{HitInfo(), HitInfo()}
{}

}// end namespace ph