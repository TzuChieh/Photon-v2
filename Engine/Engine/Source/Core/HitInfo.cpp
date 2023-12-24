#include "Core/HitInfo.h"

#include <type_traits>

namespace ph
{

// A simple value type should be trivially copyable
static_assert(std::is_trivially_copyable_v<HitInfo>);

HitInfo::HitInfo() :
	m_position(0, 0, 0),

	// TODO: default axes should change here
	m_dPdU(0, 0, 1),
	m_dPdV(1, 0, 0),
	m_dNdU(0, 0, 1),
	m_dNdV(1, 0, 0),

	m_geometryBasis(),
	m_shadingBasis()
{}

}// end namespace ph
