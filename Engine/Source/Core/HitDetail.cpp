#include "Core/HitDetail.h"

#include <limits>

namespace ph
{

HitDetail::HitDetail() :
	m_primitive(nullptr),
	m_st(0, 0),
	m_hitInfos{HitInfo(), HitInfo()}
{

}

}// end namespace ph