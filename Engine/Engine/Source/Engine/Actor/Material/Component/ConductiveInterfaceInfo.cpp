#include "Engine/Actor/Material/Component/ConductiveInterfaceInfo.h"
#include "Engine/Core/SurfaceBehavior/Property/TSchlickApproxConductorFresnel.h"
#include "Engine/Core/SurfaceBehavior/Property/ExactConductorFresnel.h"
#include "Engine/Core/SurfaceBehavior/Property/surface_property.h"

#include <Common/logging.h>

#include <utility>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(ConductiveInterfaceInfo, BSDF);

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

std::unique_ptr<ConductorFresnel> ConductiveInterfaceInfo::genFresnelEffect(const CookingContext& ctx) const
{
	using ConstantF0 = TConstantSurfaceProperty<math::Spectrum>;

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
		
			return std::make_unique<TSchlickApproxConductorFresnel<ConstantF0>>(
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

		if(m_f0Map)
		{
			using F0 = TTexturedSurfaceProperty<math::Spectrum>;
			using Fresnel = TSchlickApproxConductorFresnel<F0>;

			return std::make_unique<Fresnel>(F0(m_f0Map->genColorTexture(ctx)));
		}
		
		return std::make_unique<TSchlickApproxConductorFresnel<ConstantF0>>(m_f0);
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

void ConductiveInterfaceInfo::setF0Map(std::shared_ptr<Image> f0Map)
{
	m_f0Map = std::move(f0Map);
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
