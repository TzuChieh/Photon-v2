#pragma once

#include "Engine/Core/SurfaceBehavior/surface_optics_fwd.h"
#include "Engine/Core/LTA/SidednessAgreement.h"
#include "Engine/Core/LTA/enums.h"

#include <initializer_list>

namespace ph
{

/*! @brief The environment a BSDF query is performed under.
*/
class BsdfQueryContext final
{
public:
	SurfaceElemental        elemental = ALL_SURFACE_ELEMENTALS;
	lta::ETransport         transport = lta::ETransport::Radiance;
	lta::SidednessAgreement sidedness = lta::SidednessAgreement{lta::ESidednessPolicy::Strict};
	
	/*! If not `ALL_SURFACE_PHENOMENA`, indicates there are multiple elementals to query for. */
	SurfacePhenomena targetPhenomena = ALL_SURFACE_PHENOMENA;

	BsdfQueryContext() = default;
	explicit BsdfQueryContext(SurfaceElemental elemental);
	explicit BsdfQueryContext(lta::ESidednessPolicy sidednessPolicy);

	/*! @brief Creates an elemental-based context.
	*/
	BsdfQueryContext(
		SurfaceElemental elemental,
		lta::ETransport transport, 
		lta::ESidednessPolicy sidednessPolicy = lta::ESidednessPolicy::Strict);

	/*! @brief Creates a phenomenon-based context.
	*/
	BsdfQueryContext(
		SurfacePhenomena targetPhenomena,
		lta::ETransport transport,
		lta::ESidednessPolicy sidednessPolicy = lta::ESidednessPolicy::Strict);

	/*! @brief Creates a phenomenon-based context.
	*/
	BsdfQueryContext(
		std::initializer_list<ESurfacePhenomenon> targetPhenomena,
		lta::ETransport transport,
		lta::ESidednessPolicy sidednessPolicy = lta::ESidednessPolicy::Strict);
};

// In-header Implementations:

inline BsdfQueryContext::BsdfQueryContext(SurfaceElemental elemental)
	: BsdfQueryContext()
{
	this->elemental = elemental;
}

inline BsdfQueryContext::BsdfQueryContext(lta::ESidednessPolicy sidednessPolicy)
	: BsdfQueryContext()
{
	this->sidedness = lta::SidednessAgreement(sidednessPolicy);
}

inline BsdfQueryContext::BsdfQueryContext(
	SurfaceElemental elemental,
	lta::ETransport transport,
	lta::ESidednessPolicy sidednessPolicy)

	: elemental(elemental)
	, transport(transport)
	, sidedness(sidednessPolicy)
	, targetPhenomena(ALL_SURFACE_PHENOMENA)
{}

inline BsdfQueryContext::BsdfQueryContext(
	SurfacePhenomena targetPhenomena,
	lta::ETransport transport,
	lta::ESidednessPolicy sidednessPolicy)

	: elemental(ALL_SURFACE_ELEMENTALS)
	, transport(transport)
	, sidedness(sidednessPolicy)
	, targetPhenomena(targetPhenomena)
{}

inline BsdfQueryContext::BsdfQueryContext(
	std::initializer_list<ESurfacePhenomenon> targetPhenomena,
	lta::ETransport transport,
	lta::ESidednessPolicy sidednessPolicy)

	: BsdfQueryContext(
		SurfacePhenomena(targetPhenomena),
		transport,
		sidednessPolicy)
{}

}// end namespace ph
