#include "Actor/Material/Utility/DielectricInterfaceInfo.h"
#include "Core/SurfaceBehavior/Property/SchlickApproxDielectricFresnel.h"
#include "Core/SurfaceBehavior/Property/ExactDielectricFresnel.h"
#include "Common/logging.h"

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(DielectricInterfaceInfo, BSDF);

DielectricInterfaceInfo::DielectricInterfaceInfo() :
	DielectricInterfaceInfo(EInterfaceFresnel::Schlick, 1.0_r, 1.5_r)
{}

DielectricInterfaceInfo::DielectricInterfaceInfo(
	const EInterfaceFresnel fresnel,
	const real              iorOuter,
	const real              iorInner) :

	m_fresnel (fresnel),
	m_iorOuter(iorOuter),
	m_iorInner(iorInner)
{}

std::unique_ptr<DielectricFresnel> DielectricInterfaceInfo::genFresnelEffect() const
{
	if(m_fresnel == EInterfaceFresnel::Exact)
	{
		return std::make_unique<ExactDielectricFresnel>(
			m_iorOuter,
			m_iorInner);
	}
	else
	{
		PH_ASSERT(m_fresnel == EInterfaceFresnel::Schlick);

		return std::make_unique<SchlickApproxDielectricFresnel>(
			m_iorOuter,
			m_iorInner);
	}
}

void DielectricInterfaceInfo::setFresnel(const EInterfaceFresnel fresnel)
{
	m_fresnel = fresnel;
}

void DielectricInterfaceInfo::setIorOuter(const real iorOuter)
{
	m_iorOuter = iorOuter;
}

void DielectricInterfaceInfo::setIorInner(const real iorInner)
{
	m_iorInner = iorInner;
}

}// end namespace ph
