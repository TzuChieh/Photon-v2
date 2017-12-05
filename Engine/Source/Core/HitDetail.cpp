#include "Core/HitDetail.h"

#include <limits>

namespace ph
{

HitDetail::HitDetail() :
	m_primitive(nullptr),
	m_hitInfos{HitInfo(), HitInfo()}
{

}

}// end namespace ph