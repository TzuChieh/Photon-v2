#include "Core/HitInfo.h"

namespace ph
{

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
