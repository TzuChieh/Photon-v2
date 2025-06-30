#include "Engine/Core/HitInfo.h"

#include <type_traits>

namespace ph
{

// A simple value type should be trivially copyable
static_assert(std::is_trivially_copyable_v<HitInfo>);

namespace
{

inline bool compute_basis_from_Y_and_refZ(math::Basis3R& basis, math::Vector3R refZ)
{
	basis.setXAxis(basis.getYAxis().cross(refZ));
	basis.renormalizeXAxis();
	if(!basis.getXAxis().isFinite())
	{
		return false;
	}

	basis.setZAxis(basis.getXAxis().cross(basis.getYAxis()));
	return true;
}

inline bool compute_basis_from_Y_and_refX(math::Basis3R& basis, math::Vector3R refX)
{
	basis.setZAxis(refX.cross(basis.getYAxis()));
	basis.renormalizeZAxis();
	if(!basis.getZAxis().isFinite())
	{
		return false;
	}

	basis.setXAxis(basis.getYAxis().cross(basis.getZAxis()));
	return true;
}

}// end namespace

HitInfo::HitInfo()
	: m_pos(0, 0, 0)

	// No change in position w.r.t. UV
	, m_dPdU(0, 0, 0)
	, m_dPdV(0, 0, 0)

	// No change in normal w.r.t. UV
	, m_dNdU(0, 0, 0)
	, m_dNdV(0, 0, 0)

	, m_geometryBasis()
	, m_shadingBasis()

	, m_shadingTangent(std::nullopt)
{}

void HitInfo::computeBases()
{
	// Computation should be aware of potential numerical error and handle all edge cases
	// (vectors could be 0 or too close to each other, partial derivatives could be 0)

	// TODO: it may be worth to store a flag for shading normal like shading tangent, so we can
	// simply assign geometry basis to shading basis if shading normal is not available.

	// Geometry basis: try to align with dPdU or dPdV
	if(!compute_basis_from_Y_and_refZ(m_geometryBasis, getdPdU()) &&
	   !compute_basis_from_Y_and_refX(m_geometryBasis, getdPdV()))
	{
		m_geometryBasis = math::Basis3R::makeFromUnitY(m_geometryBasis.getYAxis());
	}

	// Shading basis

	// X axis of shading basis: try to align with tangent if available
	if(hasShadingTangent())
	{
		if(!compute_basis_from_Y_and_refZ(m_shadingBasis, getShadingTangent()))
		{
			m_shadingBasis = math::Basis3R::makeFromUnitY(m_shadingBasis.getYAxis());
		}
	}
	// Otherwise, try to align with dNdU, dNdV, dPdU, dPdV
	else
	{
		if(!compute_basis_from_Y_and_refZ(m_shadingBasis, getdNdU()) &&
		   !compute_basis_from_Y_and_refX(m_shadingBasis, getdNdV()) &&
		   !compute_basis_from_Y_and_refZ(m_shadingBasis, getdPdU()) &&
		   !compute_basis_from_Y_and_refX(m_shadingBasis, getdPdV()))
		{
			m_shadingBasis = math::Basis3R::makeFromUnitY(m_shadingBasis.getYAxis());
		}
	}

	PH_ASSERT_MSG(m_geometryBasis.getYAxis().isFinite() && m_shadingBasis.getYAxis().isFinite(), "\n"
		"m_geometryBasis.getYAxis() = " + m_geometryBasis.getYAxis().toString() + "\n"
		"m_shadingBasis.getYAxis()  = " + m_shadingBasis.getYAxis().toString() + "\n");

#if PH_DEBUG
	m_isBasesComputed = true;
#endif
}

}// end namespace ph
