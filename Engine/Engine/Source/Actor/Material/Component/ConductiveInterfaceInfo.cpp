#include "Actor/Material/Component/ConductiveInterfaceInfo.h"
#include "Core/SurfaceBehavior/Property/SchlickApproxConductorFresnel.h"
#include "Core/SurfaceBehavior/Property/ExactConductorFresnel.h"

#include <Common/logging.h>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(ConductiveInterfaceInfo, BSDF);

ConductiveInterfaceInfo::ConductiveInterfaceInfo() :
	ConductiveInterfaceInfo(math::Spectrum(1))
{}

ConductiveInterfaceInfo::ConductiveInterfaceInfo(const math::Spectrum& f0) :
	m_fresnel  (EInterfaceFresnel::Schlick),
	m_f0       (f0),
	m_iorOuter (1.0_r),
	m_iorInnerN(),
	m_iorInnerK()
{}

ConductiveInterfaceInfo::ConductiveInterfaceInfo(
	const real            iorOuter,
	const math::Spectrum& iorInnerN,
	const math::Spectrum& iorInnerK) :

	m_fresnel  (EInterfaceFresnel::Exact),
	m_f0       (1),
	m_iorOuter (iorOuter),
	m_iorInnerN(iorInnerN),
	m_iorInnerK(iorInnerK)
{}

std::unique_ptr<ConductorFresnel> ConductiveInterfaceInfo::genFresnelEffect() const
{
	// If we have enough information for an exact Fresnel description
	if(m_iorInnerN && m_iorInnerK)
	{
		if(m_fresnel == EInterfaceFresnel::Exact)
		{
			return std::make_unique<ExactConductorFresnel>(
				m_iorOuter,
				*m_iorInnerN,
				*m_iorInnerK);
		}
		else
		{
			// If Schlick approximation is explicitly requested, use exact
			// Fresnel description to derive required input.

			PH_ASSERT(m_fresnel == EInterfaceFresnel::Schlick);
		
			return std::make_unique<SchlickApproxConductorFresnel>(
				m_iorOuter,
				*m_iorInnerN,
				*m_iorInnerK);
		}
	}
	else
	{
		if(m_fresnel == EInterfaceFresnel::Exact)
		{
			// If exact Fresnel is explicitly requested, since we do not have
			// complete information for that, fallback to Schlick approximation
			// and issue a warning.

			PH_LOG(ConductiveInterfaceInfo, Warning,
				"exact Fresnel formula is requested without complete information "
				"specified; will fallback to Schlick approximation");
		}

		return std::make_unique<SchlickApproxConductorFresnel>(m_f0);
	}
}

void ConductiveInterfaceInfo::setFresnel(const EInterfaceFresnel fresnel)
{
	m_fresnel = fresnel;
}

void ConductiveInterfaceInfo::setF0(const math::Spectrum& f0)
{
	m_f0 = f0;
}

void ConductiveInterfaceInfo::setIorOuter(const real iorOuter)
{
	m_iorOuter = iorOuter;
}

void ConductiveInterfaceInfo::setIorInnerN(const math::Spectrum& iorInnerN)
{
	m_iorInnerN = iorInnerN;
}

void ConductiveInterfaceInfo::setIorInnerK(const math::Spectrum& iorInnerK)
{
	m_iorInnerK = iorInnerK;
}

}// end namespace ph
