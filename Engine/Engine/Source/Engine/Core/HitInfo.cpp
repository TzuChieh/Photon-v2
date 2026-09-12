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

	// Basis axes also hold input normals and tangent/bitangent before computation
	, m_geometryBasis()
	, m_shadingBasis()

	, m_hasShadingNormal(false)
	, m_hasShadingTangent(false)
{}

void HitInfo::computeBases()
{
	// Computation should be aware of potential numerical error and handle all edge cases
	// (vectors could be 0 or too close to each other, partial derivatives could be 0)

	// Geometry basis: try to align with dPdU or dPdV
	if(!compute_basis_from_Y_and_refZ(m_geometryBasis, getdPdU()) &&
	   !compute_basis_from_Y_and_refX(m_geometryBasis, getdPdV()))
	{
		m_geometryBasis = math::Basis3R::makeFromUnitY(getGeometryNormal());
	}

	// Shading basis

	if(!hasShadingNormal())
	{
		m_shadingBasis = m_geometryBasis;
	}
	// Align with the specified tangent frame when available
	else if(hasShadingTangent())
	{
		const math::Vector3R shadingTangent = getShadingTangent();
		const math::Vector3R shadingBitangent = getShadingBitangent();

		if(compute_basis_from_Y_and_refZ(m_shadingBasis, shadingTangent))
		{
			if(m_shadingBasis.getXAxis().dot(shadingBitangent) < 0)
			{
				m_shadingBasis.setXAxis(-m_shadingBasis.getXAxis());
			}
		}
		else if(!compute_basis_from_Y_and_refX(m_shadingBasis, shadingBitangent))
		{
			m_shadingBasis = math::Basis3R::makeFromUnitY(getShadingNormal());
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
			m_shadingBasis = math::Basis3R::makeFromUnitY(getShadingNormal());
		}
	}

	PH_ASSERT_MSG(m_geometryBasis.getYAxis().isFinite() && m_shadingBasis.getYAxis().isFinite(), "\n"
		"m_geometryBasis.getYAxis() = " + m_geometryBasis.getYAxis().toString() + "\n"
		"m_shadingBasis.getYAxis()  = " + m_shadingBasis.getYAxis().toString() + "\n");

#if PH_DEBUG
	m_areBasesComputed = true;
#endif
}

}// end namespace ph
